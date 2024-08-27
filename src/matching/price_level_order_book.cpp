#include <iostream>
#include <limits.h>
#include "map_orderbook.h"
#include "event.h"

namespace QuantaTrader {
PriceLevelOrderBook::PriceLevelOrderBook(uint32_t symbol_id, EventHandler &event_handler) 
    : symbol_id(symbol_id),
    event_handler(event_handler) {
        last_traded_price = 0;
        trailing_bid_price = 0;
        trailing_ask_price = std::numeric_limits<uint64_t>::max();
    }

void PriceLevelOrderBook::addOrder(Order order) {
    event_handler.handleOrderAdded(OrderAdded{order}); // CHECK IF THIS IS FINE
    switch (order.getType()) {
        case OrderType::MARKET:
            addMarketOrder();
            break;
        case OrderType::LIMIT:
            addLimitOrder();
            break;
        case OrderType::STOP:
        case OrderType::STOP_LIMIT:
        OrderType::TRAILING_STOP:
        OrderType::TRAILING_STOP_LIMIT:
            addStopOrder();
            break;
    }
    activateStopOrders();
}

void PriceLevelOrderBook::deleteOrder(uint64_t order_id) override {
    auto orders_it = orders.find(order_id);
    auto &levels_it = orders_it->second.level_it;
    Order &order_to_delete = orders_it->second.order;
    event_handler.handleOrderDeleted(OrderDeleted{orders_it->second.order});
    levels_it->second.deleteOrder(order_to_delete);
    if (levels_it->second.empty()) {
        // delete from appropriate order side the relevant order type
        switch (order_to_delete.getType()) {
            bool isAsk = order.getSide() == OrderSide::ASK;
            case OrderType::LIMIT:
                if (isAsk) {
                    ask_levels.erase(levels_it);
                } else {
                    bid_levels.erase(levels_it);
                }
                break;
            case OrderType::STOP:
            case OrderType::STOP_LIMIT:
                if (isAsk) {
                    stop_ask_levels.erase(levels_it);
                } else {
                    stop_bid_levels.erase(levels_it);
                }
                break;
            OrderType::TRAILING_STOP:
            OrderType::TRAILING_STOP_LIMIT:
                if (isAsk) {
                    trailing_stop_ask_levels.erase(levels_it);
                } else {
                    trailing_stop_bid_levels.erase(levels_it);
                }
                break;
        }
    }
    orders.erase(orders_it);
    activateStopOrders();
}

void PriceLevelOrderBook::modifyOrder(uint64_t order_id, uint64_t new_order_id, uint64_t new_price) override {
    auto orders_it = orders.find(order_id);
    Order new_order = orders_it->second.order;
    new_order.setId(new_order_id);
    new_order.setPrice(new_price);
    deleteOrder(order_id);
    addOrder(new_order);
    activateStopOrders();
}

void PriceLevelOrderBook::cancelOrder(uint64_t order_id, uint64_t quantity) override {
    auto orders_it = orders.find(order_id);
    auto &level_it = orders_it->second.level_it;
    Level &level_to_cancel = level_it->second;
    Order &order_to_cancel = orders_it->second.order;
    uint64_t quantity_before_cancel = order_to_cancel.getOpenQuantity();
    order_to_cancel.setQuantity(quantity);
    event_handler.handleOrderUpdated(OrderUpdated{order_to_cancel});
    level_to_cancel.reduceVolume(quantity_before_cancel - order_to_cancel.getOpenQuantity());
    if (order_to_cancel.getOpenQuantity() == 0) {
        deleteOrder(order_id, true);
    }
    activateStopOrders();
}

void PriceLevelOrderBook::executeOrder(uint64_t order_id, uint64_t quantity, uint64_t price) override {
    auto orders_it = orders.find(order_id);
    Order &order_to_execute = orders_it->second.order;
    uint64_t executing_quantity = std::min(quantity, order_to_execute.getOpenQuantity());
    order_to_execute.execute(price, executing_quantity);
    last_traded_price = price;
    event_handler.handleOrderExecuted(OrderExecuted{order_to_execute});
    Level &level_to_execute = orders_it->second.level_it->second;
    level_to_execute.reduceVolume(order_to_execute.getLastExecutedQuantity());
    if (order_to_execute.getOpenQuantity() == 0) {
        deleteOrder(order_id, true);
    }
    activateStopOrders();
}

void PriceLevelOrderBook::executeOrder(uint64_t order_id, uint64_t quantity) override {
    auto orders_it = orders.find(order_id);
    Order &order_to_execute = orders_it->second.order;
    uint64_t executing_quantity = std::min(quantity, order_to_execute.getOpenQuantity());
    uint64_t executing_price = order_to_execute.getPrice();
    order_to_execute.execute(executing_price, executing_quantity);
    last_traded_price = executing_price;
    event_handler.handleOrderExecuted(OrderExecuted{order_to_execute});
    Level &level_to_execute = orders_it->second.level_it->second;
    level_to_execute.reduceVolume(order_to_execute.getLastExecutedQuantity());
    if (order_to_execute.getOpenQuantity() == 0) {
        deleteOrder(order_id, true);
    }
    activateStopOrders();
}

void PriceLevelOrderBook::addMarketOrder(Order &order) {
    // Market orders are traded instantly at the best available price
    if (order.getSide() == OrderSide::ASK) {
        order.setPrice(0)
    } else {
        order.setPrice(std::numeric_limits<uint64_t>::max());
    }
    match(order); // matching takes care of deleting the order as well
    event_handler.handleOrderDeleted(OrderDeleted{order});
}

void PriceLevelOrderBook::addLimitOrder(Order &order) {
    match(order);
    // IOC and FOK orders need to be executed immediately
    if (order.getOpenQuantity() != 0 && order.getTimeInForce() != OrderTimeInForce::IOC && order.getTimeInForce() != OrderTimeInForce::FOK) {
        insertLimitOrder(order);
    } else {
        event_handler.handleOrderDeleted(OrderDeleted{order});
    }
}

void PriceLevelOrderBook::insertLimitOrder(const Order &order) {
    if (order.getSide() == OrderSide::ASK) {
        // using C++17 structured binding to hold the return value from emplace() 
        // first value is an iterator, second value is a boolean indicating whether emplace was successful
        auto [level_it, inserted] = ask_levels.emplace(order.getPrice(), Level(order.getPrice(), LevelSide::Ask, symbol_id));
        auto [orders_it, success] = orders.emplace(order.getOrderID(), OrderWithLevelIterator{order, level_it});
        level_it->second.addOrder(orders_it->second.order);
    }
    else {
        auto [level_it, inserted] = bid_levels.emplace(order.getPrice(), Level(order.getPrice(), LevelSide::Bid, symbol_id));
        auto [orders_it, success] = orders.emplace(order.getOrderID(), OrderWithLevelIterator{order, level_it});
        level_it->second.addOrder(orders_it->second.order);
    }
}

void PriceLevelOrderBook::addStopOrder(Order &order) {
    if (order.getType() == OrderType::TRAILING_STOP || order.getType() == OrderType::TRAILING_STOP_LIMIT) {
        calculateStopPrice(order);
    }
    // get market price at which the stock traded last
    uint64_t market_price = 0;
    if (order.getSide() == OrderSide::ASK) {
        market_price = lastTradedPriceBid();
    } else {
        market_price = lastTradedPriceAsk();
    }
    uint64_t order_stop_price = order.getStopPrice();
    // the stop order can be matched if market price <= stop price for ask orders
    //                           and if market price >= stop price for bid orders
    bool match = false;
    if (order.getSide() == OrderSide::ASK && market_price <= order_stop_price) {
        match = true;
    } else if (order.getSide() == OrderSide::BID && market_price >= order_stop_price) {
        match = true;
    }
    if (match) {
        if (order.getType() == OrderType::STOP || order.getType() == OrderType::TRAILING_STOP) {
            order.setType(OrderType::Market);
        } else { // for STOP_LIMIT and TRAILING_STOP_LIMIT orders
            order.setType(OrderType::Limit);
        }
        order.setStopPrice(0);
        order.setTrailAmount(0);
        event_handler.handleOrderUpdated(OrderUpdated{order});
        if (order.getType() == OrderType::MARKET) {
            addMarketOrder(order)
        } else {
            addLimitOrder(order);
        }
        return;
    }
    if (order.getType() == OrderType::TRAILING_STOP || order.getType() == OrderType::TRAILING_STOP_LIMIT) {
        insertTrailingStopOrder(order);
    } else {
        insertStopOrder(order);
    }
}

void PriceLevelOrderBook::insertStopOrder(const Order &order) {

}

void PriceLevelOrderBook::insertTrailingStopOrder(const Order &order) {

}

uint64_t PriceLevelOrderBook::calculateStopPrice(Order &order) {

}

void PriceLevelOrderBook::updateBidStopOrders() {

}

void PriceLevelOrderBook::updateAskStopOrders() {

}

void PriceLevelOrderBook::activateStopOrders() {

}

bool PriceLevelOrderBook::activateBidStopOrders() {

}

bool PriceLevelOrderBook::activateAskStopOrders() {

}

void PriceLevelOrderBook::activateStopOrder(Order order) {

}

void PriceLevelOrderBook::match(Order &order) {

}

void PriceLevelOrderBook::canMatchOrder(Order &order) const {
    
}

void PriceLevelOrderBook::executeOrders(Order &ask, Order &bid, uint64_t executing_price) {

}


}
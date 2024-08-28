#include <iostream>
#include <limits.h>
#include "map_orderbook.h"
#include "event.h"

namespace QuantaTrader {
PriceLevelOrderBook::PriceLevelOrderBook(uint32_t symbol_id, EventHandler &event_handler) 
    : symbol_id(symbol_id),
    event_handler(event_handler) {
        last_traded_price = 0;
        trailing_buy_price = 0;
        trailing_sell_price = std::numeric_limits<uint64_t>::max();
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
            bool isSell = order.getSide() == OrderSide::SELL;
            case OrderType::LIMIT:
                if (isSell) {
                    sell_levels.erase(levels_it);
                } else {
                    buy_levels.erase(levels_it);
                }
                break;
            case OrderType::STOP:
            case OrderType::STOP_LIMIT:
                if (isSell) {
                    stop_sell_levels.erase(levels_it);
                } else {
                    stop_buy_levels.erase(levels_it);
                }
                break;
            OrderType::TRAILING_STOP:
            OrderType::TRAILING_STOP_LIMIT:
                if (isSell) {
                    trailing_stop_sell_levels.erase(levels_it);
                } else {
                    trailing_stop_buy_levels.erase(levels_it);
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
        deleteOrder(order_id);
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
        deleteOrder(order_id);
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
        deleteOrder(order_id);
    }
    activateStopOrders();
}

void PriceLevelOrderBook::addMarketOrder(Order &order) {
    // Market orders are traded instantly at the best available price
    if (order.getSide() == OrderSide::SELL) {
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
    if (order.getSide() == OrderSide::SELL) {
        // using C++17 structured binding to hold the return value from emplace() 
        // first value is an iterator, second value is a boolean indicating whether emplace was successful
        auto [level_it, inserted] = sell_levels.emplace(order.getPrice(), Level(order.getPrice(), LevelSide::SELL, symbol_id));
        auto [orders_it, success] = orders.emplace(order.getId(), OrderWithLevelIterator{order, level_it});
        level_it->second.addOrder(orders_it->second.order);
    }
    else {
        auto [level_it, inserted] = buy_levels.emplace(order.getPrice(), Level(order.getPrice(), LevelSide::BUY, symbol_id));
        auto [orders_it, success] = orders.emplace(order.getId(), OrderWithLevelIterator{order, level_it});
        level_it->second.addOrder(orders_it->second.order);
    }
}

void PriceLevelOrderBook::addStopOrder(Order &order) {
    if (order.getType() == OrderType::TRAILING_STOP || order.getType() == OrderType::TRAILING_STOP_LIMIT) {
        calculateStopPrice(order);
    }
    // get market price at which the stock traded last
    uint64_t market_price = 0;
    if (order.getSide() == OrderSide::SELL) {
        market_price = lastTradedBuyPrice();
    } else {
        market_price = lastTradedSellPrice();
    }
    uint64_t order_stop_price = order.getStopPrice();
    // the stop order can be matched if market price <= stop price for sell orders
    //                           and if market price >= stop price for buy orders
    bool match = false;
    if (order.getSide() == OrderSide::SELL && market_price <= order_stop_price) {
        match = true;
    } else if (order.getSide() == OrderSide::BUY && market_price >= order_stop_price) {
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
    auto stop_price = order.getStopPrice();
    auto order_id = order.getId();

    if (order.getType() == OrderType::SELL) {
        auto level_it = stop_sell_levels.emplace(
            std::piecewise_construct, // to avoid unnecessary copying, gives better performance
            std::make_tuple(stop_price),
            std::make_tuple(stop_price, LevelSide::SELL, symbol_id)
        ).first;
        
        auto [orders_it, success] = orders.emplace(
            order_id, 
            OrderWithLevelIterator{order, level_it}
        );
        level_it->second.addOrder(orders_it->second.order);
    } else {
        auto level_it = stop_buy_levels.emplace(
            std::piecewise_construct,
            std::make_tuple(stop_price),
            std::make_tuple(stop_price, LevelSide::BUY, symbol_id)
        ).first;
        
        auto [orders_it, success] = orders.emplace(
            order_id, 
            OrderWithLevelIterator{order, level_it}
        );
        level_it->second.addOrder(orders_it->second.order);
    }
}

void PriceLevelOrderBook::insertTrailingStopOrder(const Order &order) {
    auto stop_price = order.getStopPrice();
    auto order_id = order.getId();

    if (order.getType() == OrderType::SELL) {
        auto level_it = trailing_stop_sell_levels.emplace(
            std::piecewise_construct, // to avoid unnecessary copying, gives better performance
            std::make_tuple(stop_price),
            std::make_tuple(stop_price, LevelSide::SELL, symbol_id)
        ).first;
        
        auto [orders_it, success] = orders.emplace(
            order_id, 
            OrderWithLevelIterator{order, level_it}
        );
        level_it->second.addOrder(orders_it->second.order);
        orders_it->second.level_it = level_it;
    } else {
        auto level_it = trailing_stop_buy_levels.emplace(
            std::piecewise_construct,
            std::make_tuple(stop_price),
            std::make_tuple(stop_price, LevelSide::BUY, symbol_id)
        ).first;
        
        auto [orders_it, success] = orders.emplace(
            order_id, 
            OrderWithLevelIterator{order, level_it}
        );
        level_it->second.addOrder(orders_it->second.order);
        orders_it->second.level_it = level_it;
    }
}

uint64_t PriceLevelOrderBook::calculateStopPrice(Order &order) {
    uint64_t trail_amount = order.getTrailAmount();
    if (order.getType() == OrderType::SELL) {
        uint64_t market_price = lastTradedBuyPrice();
        // if trail amount >= market price, set stop price to 0
        uint64_t new_stop_price = 0;
        if (trail_amount < market_price) {
            new_stop_price = market_price - trail_amount;
        }
        order.setStopPrice(new_stop_price);
        return new_stop_price;
    }
    else
    {
        uint64_t market_price = lastTradedSellPrice();
        uint64_t = new_stop_price = 0;
        // set new stop price depending on check for overflow on int 64 when adding market_price and trail_amount
        if (market_price < (std::numeric_limits<uint64_t>::max() - trail_amount)) {
            new_stop_price = market_price + trail_amount
        } else {
            new_stop_price = std::numeric_limits<uint64_t>::max()
        }
        order.setStopPrice(new_stop_price);
        return new_stop_price;
    }
}

void PriceLevelOrderBook::updateTrailingBuyStopOrders() {
    // if the trailing sell price > last traded sell price, market has moved in the favorable direction for trailing 
    // stop buy orders, we need to update their stop price
    if (trailing_sell_price > lastTradedSellPrice() && !trailing_stop_buy_levels.empty()) {
        std::map<uint64_t, Level> updated_trailing_levels; // temp map for updated levels 
        for (auto& [level_price, level] : trailing_stop_buy_levels) {
            // for each order in the level, calculate the new price, remove the previous order and add the new order
            while (!level.empty()) {
                Order& stop_order = level.front();
                uint64_t new_stop_price = calculateStopPrice(stop_order);

                auto updated_level_it = updated_trailing_levels.emplace(
                    std::make_tuple(new_stop_price), std::make_tuple(new_stop_price, LevelSide::BUY, symbol_id)).first;
                
                orders[stop_order.getId()].level_it = updated_level_it;
                level.popFront();
                updated_level_it->second.addOrder(stop_order);
                event_handler.handleOrderUpdated(OrderUpdated{stop_order});
            }
        }
        // swap the old levels with the new ones
        std::swap(trailing_stop_buy_levels, updated_trailing_levels);
    } else {
        trailing_sell_price = last_traded_price;
    }
}

void PriceLevelOrderBook::updateTrailingSellStopOrders() {
    // if the trailing buy price < last traded buy price, market has moved in a favorable direction for trailing
    // stop sell orders, we need to update their stop price
    if (trailing_buy_price < lastTradedBuyPrice() && !trailing_stop_sell_levels.empty()) {
        std::map<uint64_t, Level> updated_trailing_levels;
        for (auto& [level_price, level] : trailing_stop_sell_levels) {
            while (!level.empty()) {
                Order& stop_order = level.front();
                uint64_t new_stop_price = calculateStopPrice(stop_order);

                auto updated_level_it = updated_trailing_levels.emplace(
                    std::make_tuple(new_stop_price), std::make_tuple(new_stop_price, LevelSide::SELL, symbol_id)).first;
                
                orders[stop_order.getId()].level_it = updated_level_it;
                level.popFront();
                updated_level_it->second.addOrder(stop_order);
                event_handler.handleOrderUpdated(OrderUpdated{stop_order});
            }
        }
        // swap the old levels with the new ones
        std::swap(trailing_stop_sell_levels, updated_trailing_levels);
    } else {
        trailing_buy_price = last_traded_price;
    }
}

void PriceLevelOrderBook::activateStopOrders() {
    // we keep going till all required stop orders have been activated, since activating some orders can 
    // cause prices to change which inturn activates more stop orders
    bool continue = true;
    while (continue) {
        continue = activateBuyStopOrders();
        updateTrailingSellStopOrders();
        continue = activateSellStopOrders();
        updateTrailingBuyStopOrders();
    }
}

bool PriceLevelOrderBook::activateBuyStopOrders() {
    bool activated_orders = false;
    // stop order handling
    auto stop_levels_it = stop_buy_levels.begin();
    uint64_t last_sell_price = lastTradedSellPrice();
    // if the current stop level price <= last sell price, it is eligible for activation
    while (stop_levels_it != stop_buy_levels.end() && stop_levels_it->first <= last_sell_price) {
        activated_orders = true;
        Order &stop_order = stop_levels_it->second.front();
        activateStopOrder(stop_order);
        // reset level iterator to the beginning to account for any changes in the order book caused by this new activation
        stop_levels_it = stop_buy_levels.begin();
    }
    // trailing stop order handling
    auto trailing_stop_levels_it = trailing_stop_buy_levels.begin();
    last_sell_price = lastTradedSellPrice();
    while (trailing_stop_levels_it != trailing_stop_buy_levels.end() && trailing_stop_levels_it->first <= last_sell_price) {
        activated_orders = true;
        Order &trailing_stop_order = trailing_stop_levels_it->second.front();
        activateStopOrder(trailing_stop_order);
        trailing_stop_levels_it = trailing_stop_buy_levels.begin();
    }
    return activated_orders;
}

bool PriceLevelOrderBook::activateSellStopOrders() {
    bool activated_orders = false;
    uint64_t last_buy_price = lastTradedBuyPrice();
    // 
    auto stop_levels_it = stop_sell_levels.rbegin();
    // if the current stop level price >= last buy price, it is eligible for activation
    while (stop_levels_it != stop_sell_levels.rend() && stop_levels_it->first >= last_buy_price) {
        activated_orders = true;
        Order &stop_order = stop_levels_it->second.front();
        activateStopOrder(stop_order);
        // reset level iterator to the beginning to account for any changes in the order book caused by this new activation
        stop_levels_it = stop_sell_levels.rbegin();
    }
    auto trailing_stop_levels_it = trailing_stop_sell_levels.rbegin();
    last_buy_price = lastTradedBuyPrice();
    while (trailing_stop_levels_it != trailing_stop_sell_levels.rend() && trailing_stop_levels_it->first >= last_buy_price) {
        activated_orders = true;
        Order &trailing_stop_order = trailing_stop_levels_it->second.front();
        activateStopOrder(trailing_stop_order);
        trailing_stop_levels_it = trailing_stop_sell_levels.rbegin();
    }
    return activated_orders;
}

// deletes the stop order, instead adds a new market or limit order depending of stop order type
void PriceLevelOrderBook::activateStopOrder(Order order) {
    deleteOrder(order.getId());
    order.setStopPrice(0);
    order.setTrailAmount(0);
    if (order.getType() == OrderType::STOP || order.getType() == OrderType::TRAILING_STOP) {
        order.setType(OrderType::MARKET);
        event_handler.handleOrderUpdated(OrderUpdated{order});
        addMarketOrder(order);
    }
    else {
        order.setType(OrderType::LIMIT);
        event_handler.handleOrderUpdated(OrderUpdated{order});
        addLimitOrder(order);
    }
}

void PriceLevelOrderBook::match(Order &order) {
    //  if order is fill or kill and cannot be filled, nothing happens
    if (order.getTimeInForce == OrderTimeInForce::FOK && !canMatchOrder(order)) {
        return;
    }
    if (order.getType() == OrderSide::SELL) {
        // since we have a sell order, we would want to match it to some buy order (highest price first)
        auto buy_levels_it = buy_levels.rbegin();
        Order &sell_order = order;
        // while the highest buy price >= price of the sell order and the sell order still has open quantity
        while (buy_levels_it != buy_levels.rend() && buy_levels_it->first >= sell_order.getPrice() && sell_order.getOpenQuantity() != 0) {
            Level &buy_level = buy_levels_it->second;
            // get first buy order
            Order &buy_order = buy_level.front();
            uint64_t executing_price = buy_order.getPrice();
            // sell order is matched with the top buy order in the current level
            executeOrders(sell_order, buy_order, executing_price);
            buy_level.reduceVolume(buy_order.getLastExecutedQuantity());
            // remove buy order if its now filled
            if (buy_order.getOpenQuantity() == 0)
                deleteOrder(buy_order.getId());
            // reset level iterator to the beginning to account for any changes in the order book caused by this new execution
            buy_levels_it = buy_levels.rbegin();
        }
    }
    if (order.getSide() == OrderSide::BUY) {
        // since we have a buy order, we would want to match it to some sell order (lowest price first)
        auto sell_levels_it = sell_levels.begin();
        Order &buy_order = order;
        // while the lowest sell price <= price of the buy order and the buy order still has open quantity
        while (sell_levels_it != sell_levels.end() && sell_levels_it->first <= buy_order.getPrice() && buy_order.getOpenQuantity() != 0) {
            Level &sell_level = sell_levels_it->second;
            // get first sell order
            Order &sell_order = sell_level.front();
            uint64_t executing_price = sell_order.getPrice();
            // buy order is matched with the top sell order in the current level
            executeOrders(sell_order, buy_order, executing_price);
            sell_level.reduceVolume(sell_order.getLastExecutedQuantity());
            // remove the sell order if its now filled
            if (sell_order.getOpenQuantity() == 0)
                deleteOrder(sell_order.getId());
            // reset level iterator to the beginning to account for any changes in the order book caused by this new execution
            sell_levels_it = sell_levels.begin();
        }
    }
}

// helper method to see whether a FOK order can be matched
bool PriceLevelOrderBook::canMatchOrder(Order &order) const {
    uint64_t price = order.getPrice();
    uint64_t quantity_required = order.getOpenQuantity();
    uint64_t quantity_available = 0; // accumulator to see how much maximum quantity can be matched for this order
    if (order.getSide() == OrderSide::SELL) {
        // since we have a sell order, we want to match it to buy orders (highest price first)
        auto buy_levels_it = buy_levels.rbegin();
        // while buy level's price is greater than or equal to the sell order's price
        while (buy_levels_it != buy_levels.rend() && buy_levels_it->first >= price) {
            // check whether the level has enough quantity to fill this order
            uint64_t level_volume = buy_levels_it->second.getVolume();
            uint64_t quantity_needed = quantity_required - quantity_available;
            quantity_available += std::min(level_volume, quantity_needed);
            if (quantity_available >= quantity_required)
                return true;
        }
    }
    else {
        // since we have a buy order, we want to match it to a sell order (lowest price first)
        auto sell_levels_it = sell_levels.begin();
        // while the sell level's price is lower than or equal to the buy order's price
        while (sell_levels_it != sell_levels.end() && sell_levels_it->first <= price) {
            // check whether the level has enough quantity to fill this order
            uint64_t level_volume = sell_levels_it->second.getVolume();
            uint64_t quantity_needed = quantity_required - quantity_available;
            quantity_available += std::min(level_volume, quantity_needed);
            if (quantity_available >= quantity_required)
                return true;
        }
    }
    return false;
}

void PriceLevelOrderBook::executeOrders(Order &sell, Order &buy, uint64_t executing_price) {
    // maximum quantity that can be matched between the 2 orders
    uint64_t quantity = std::min(sell.getOpenQuantity(), buy.getOpenQuantity());
    buy.execute(executing_price, quantity);
    sell.execute(executing_price, quantity);
    event_handler.handleOrderExecuted(OrderExecuted{buy});
    event_handler.handleOrderExecuted(OrderExecuted{sell});
    last_traded_price = executing_price;
}


}
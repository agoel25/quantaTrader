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
        trailing_ask_price = INT_MAX;
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

}

void PriceLevelOrderBook::executeOrder(uint64_t order_id, uint64_t quantity) override {

}

void PriceLevelOrderBook::addMarketOrder(Order &order) {

}

void PriceLevelOrderBook::insertMarketOrder(const Order &order) {

}

void PriceLevelOrderBook::addLimitOrder(Order &order) {

}

void PriceLevelOrderBook::insertLimitOrder(const Order &order) {

}

void PriceLevelOrderBook::addStopOrder(Order &order) {

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

void PriceLevelOrderBook::executeOrders(Order &ask, Order &bid, uint64_t executing_price) {

}


}
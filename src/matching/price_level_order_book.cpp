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

}

void PriceLevelOrderBook::modifyOrder(uint64_t order_id, uint64_t new_order_id, uint64_t new_price) override {

}

void PriceLevelOrderBook::cancelOrder(uint64_t order_id, uint64_t quantity) override {

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

void PriceLevelOrderBook::executeOrders(Order &buy, Order &sell, uint64_t executing_price) {

}


}
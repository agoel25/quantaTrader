#include <iostream>
#include "level.h"
#include "order.h"

namespace QuantaTrader {
Level::Level(uint64_t price, LevelSide side, uint32_t symbol_id) {
    this->price = price;
    this->side = side;
    this->symbol_id = symbol_id;
    this->volume = 0;
}

const list<Order> &Level::getOrders() const {
    return orders;
}

list<Order> &Level::getOrders() const {
    return orders;
}

Order &Level::front() {
    assert(!orders.empty());
    return orders.front();
}

Order &Level::back() {
    assert(!orders.empty());
    return orders.back();
}

void Level::removeFront() {
    assert(!orders.empty());
    Order &remove = orders.front();
    volume -= remove.getOpenQuantity();
    orders.pop_front();
}

void Level::removeBack() {
    assert(!orders.empty());
    Order &remove = orders.back();
    volume -= remove.getOpenQuantity();
    order.pop_back();
}

void addOrder(Order &order) {
    if (order.getSide() == OrderSide::BUY) {
        assert(side == LevelSide::BUY);
    } else {
        assert(side == LevelSide::SELL);
    }
    assert(order.getSymbolID() == symbol_id);
    volume += order.getOpenQuantity();
    orders.push_back(order);
}

void deleteOrder(const Order &order) {
    volume -= order.getOpenQuantity();
    orders.erase(boost::intrusive::list<Order>::s_iterator_to(order));
}

void updateVolume(uint64_t amount) {

}

}
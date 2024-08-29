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

list<Order> &Level::getOrders() {
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
    orders.pop_back();
}

void Level::addOrder(Order &order) {
    if (order.getSide() == OrderSide::SELL) {
        assert(side == LevelSide::SELL);
    } else {
        assert(side == LevelSide::BUY);
    }
    assert(order.getSymbolId() == symbol_id);
    volume += order.getOpenQuantity();
    orders.push_back(order);
}

void Level::deleteOrder(const Order &order) {
    volume -= order.getOpenQuantity();
    orders.erase(boost::intrusive::list<Order>::s_iterator_to(order));
}

void Level::reduceVolume(uint64_t amount) {
    assert(volume >= amount);
    volume -= amount;
}

void Level::popFront() {
    Order &order_to_remove = orders.front();
    volume -= order_to_remove.getOpenQuantity();
    orders.pop_front();
};

void Level::popBack() {
    Order &order_to_remove = orders.back();
    volume -= order_to_remove.getOpenQuantity();
    orders.pop_back();
}

std::string Level::toString() const {
    std::string out;
    out += std::to_string(price) + " | " + std::to_string(volume) + "\n";
    return out;
}

std::ostream &operator<<(std::ostream &os, const Level &level) {
    os << level.toString();
    return os;
}
}
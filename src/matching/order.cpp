#include <iostream>
#include "order.h"

namespace QuantaTrader {

// Order constructor
Order::Order(uint64_t id, OrderType type, OrderSide side, OrderTimeInForce time_in_force, uint32_t symbol_id, uint64_t price, uint64_t stop_price, 
    uint64_t trail_amount, uint64_t quantity, std::chrono::time_point<std::chrono::high_resolution_clock> timestamp)
    : id(id),
    type(type), 
    side(side), 
    time_in_force(time_in_force), 
    symbol_id(symbol_id), 
    price(price), 
    stop_price(stop_price), 
    trail_amount(trail_amount), 
    quantity(quantity), 
    timestamp(timestamp) {
        last_executed_price = 0;
        executed_quantity = 0;
        open_quantity = quantity;
        last_executed_price = 0;
        last_executed_quantity = 0;
    }

// Market Orders
Order Order::marketSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::MARKET, OrderSide::SELL, time_in_force, symbol_id, 0, 0, 0, quantity, std::chrono::high_resolution_clock::now());
}

Order Order::marketBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::MARKET, OrderSide::BUY, time_in_force, symbol_id, 0, 0, 0, quantity, std::chrono::high_resolution_clock::now());
}

// Limit Orders
Order Order::limitSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::LIMIT, OrderSide::SELL, time_in_force, symbol_id, price, 0, 0, quantity, std::chrono::high_resolution_clock::now());
}

Order Order::limitBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::LIMIT, OrderSide::BUY, time_in_force, symbol_id, price, 0, 0, quantity, std::chrono::high_resolution_clock::now());
}

// Stop Orders
Order Order::stopSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::STOP, OrderSide::SELL, time_in_force, symbol_id, 0, stop_price, 0, quantity, std::chrono::high_resolution_clock::now());
}

Order Order::stopSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::STOP, OrderSide::BUY, time_in_force, symbol_id, 0, stop_price, 0, quantity, std::chrono::high_resolution_clock::now());
}

// Stop Limit Orders
Order Order::stopLimitSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::STOP_LIMIT, OrderSide::SELL, time_in_force, symbol_id, price, stop_price, 0, quantity, std::chrono::high_resolution_clock::now());
}

Order Order::stopLimitBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::STOP_LIMIT, OrderSide::BUY, time_in_force, symbol_id, price, stop_price, 0, quantity, std::chrono::high_resolution_clock::now());
}

// Trailing Stop Orders
Order Order::trailingStopSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::TRAILING_STOP, OrderSide::SELL, time_in_force, symbol_id, 0, 0, trail_amount, quantity, std::chrono::high_resolution_clock::now());
}

Order Order::trailingStopBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::TRAILING_STOP, OrderSide::BUY, time_in_force, symbol_id, 0, 0, trail_amount, quantity, std::chrono::high_resolution_clock::now());
}

// Trailing Stop Limit Orders
Order Order::trailingStopLimitSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::TRAILING_STOP_LIMIT, OrderSide::SELL, time_in_force, symbol_id, price, 0, trail_amount, quantity, std::chrono::high_resolution_clock::now());
}

Order Order::trailingStopLimitBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force) {
    return Order(order_id, OrderType::TRAILING_STOP_LIMIT, OrderSide::BUY, time_in_force, symbol_id, price, 0, trail_amount, quantity, std::chrono::high_resolution_clock::now());
}

std::string typeToString(OrderType type) {
    switch (type) {
        case OrderType::MARKET:
            return "MARKET";
        case OrderType::LIMIT:
            return "LIMIT";
        case OrderType::STOP:
            return "STOP";
        case OrderType::STOP_LIMIT:
            return "STOP_LIMIT";
        case OrderType::TRAILING_STOP:
            return "TRAILING_STOP";
        case OrderType::TRAILING_STOP_LIMIT:
            return "TRAILING_STOP_LIMIT";
        default:
            return "UNKNOWN";
    }
}

std::string sideToString(OrderSide side) {
    switch (side) {
        case OrderSide::SELL:
            return "SELL";
        case OrderSide::BUY:
            return "BUY";
        default:
            return "UNKNOWN";
    }
}

std::string timeInForceToString(OrderTimeInForce time_in_force) {
    switch (time_in_force) {
        case OrderTimeInForce::GTC:
            return "GTC";
        case OrderTimeInForce::IOC:
            return "IOC";
        case OrderTimeInForce::AON:
            return "AON";
        case OrderTimeInForce::FOK:
            return "FOK";
        default:
            return "UNKNOWN";
    }
}

std::string Order::toString() const {
    std::ostringstream oss;

    // Convert timestamp to a time format string
    auto time_t = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::to_sys(timestamp));
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");

    oss << "Order [ID: " << id 
        << ", Type: " << typeToString(type) 
        << ", Side: " << sideToString(side) 
        << ", TIF: " << timeInForceToString(time_in_force)
        << ", Symbol ID: " << symbol_id 
        << ", Price: " << price 
        << ", Quantity: " << quantity 
        << ", Open Quantity: " << open_quantity
        << ", Timestamp: " << oss.str() << "]";

    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const Order &order) {
    os << order.toString();
    return os;
}
}
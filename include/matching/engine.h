#ifndef QUANTA_TRADER_ENGINE_H
#define QUANTA_TRADER_ENGINE_H
#include <iostream>
#include <fstream>
#include <memory>
#include "order.h"
#include "order_book.h"
#include "symbol.h"
#include "event_handler.h"

namespace QuantaTrader {

class EventHandler;

struct OrderBookHandler {
    explicit OrderBookHandler(std::unique_ptr<EventHandler> event_handler);

    void addOrderBook(uint32_t symbol_id, std::string symbol_name);
    void deleteOrderBook(uint32_t symbol_id, std::string symbol_name);

    // order functions with additional symbol_id to identify the order_book it is a part of
    void addOrder(const Order &order);
    void deleteOrder(uint32_t symbol_id, uint64_t order_id);
    void cancelOrder(uint32_t symbol_id, uint64_t order_id, uint64_t cancelled_quantity);
    void modifyOrder(uint32_t symbol_id, uint64_t order_id, uint64_t new_order_id, uint64_t new_price);
    void executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity, uint64_t price);
    void executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity);

    std::string toString();
};

class Engine {
public:

private:

};
}

#endif QUANTA_TRADER_ENGINE_H
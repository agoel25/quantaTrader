#ifndef QUANTA_TRADER_ENGINE_H
#define QUANTA_TRADER_ENGINE_H
#include <iostream>
#include <fstream>
#include <memory>
#include "robin_hood.h"
#include "order.h"
#include "order_book.h"
#include "symbol.h"
#include "event_handler.h"

namespace QuantaTrader {

class EventHandler;

struct OrderBookHandler {
public:
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

private:
    std::unordered_map<uint32_t, std::unique_ptr<OrderBook>> symbol_to_order_book;
    std::unique_ptr<EventHandler> event_handler;
};

class Engine {
public:
    Engine(Engine &&other) = delete;
    Engine &operator=(Engine &&other) = delete;

    // Constructor for the engine, using the event_handler as the basis
    explicit Engine(std::unique_ptr<EventHandler> event_handler);

    // adds a new symbol and its order book to the engine
    void addSymbol(uint32_t symbol_id, const std::string &symbol_name);

    // removes a symbol and its order book from the engine
    void deleteSymbol(uint32_t symbol_id);

    bool hasSymbol(uint32_t symbol_id) const;
    void addOrder(const Order &order);
    void deleteOrder(uint32_t symbol_id, uint64_t order_id);
    void cancelOrder(uint32_t symbol_id, uint64_t order_id, uint64_t cancelled_quantity);
    void modifyOrder(uint32_t symbol_id, uint64_t order_id, uint64_t new_order_id, uint64_t new_price);
    void executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity, uint64_t price);
    void executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity);
    std::string toString() const;

    // Exports the engine to a specified path in txt format
    void exportEngine(const std::string &name) const;

    friend std::ostream &operator<<(std::ostream &os, const Engine &engine);

private:
    std::unique_ptr<OrderBookHandler> orderbook_handler;
    robin_hood::unordered_map<uint32_t, std::unique_ptr<Symbol>> symbol_id_to_symbol;
};
}

#endif // QUANTA_TRADER_ENGINE_H
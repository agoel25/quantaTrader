#include "engine.h"
#include "price_level_order_book.h"

namespace QuantaTrader {
OrderBookHandler::OrderBookHandler(std::unique_ptr<EventHandler> event_handler): event_handler_(std::move(event_handler)) {}


void OrderBookHandler::addOrderBook(uint32_t symbol_id, std::string symbol_name) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it != symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol already exists in the book");
    }
    symbol_to_order_book.insert({symbol_id, std::make_unique<MapOrderBook>(symbol_id, *event_handler)});
    SymbolAdded symbol_added_event(symbol_id, std::move(symbol_name));
    event_handler->handleSymbolAdded(symbol_added_event);
}

void OrderBookHandler::deleteOrderBook(uint32_t symbol_id, std::string symbol_name) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    symbol_to_order_book.erase(it);
    SymbolDeleted symbol_deleted_event(symbol_id, std::move(symbol_name));
    event_handler->handleSymbolDeleted(symbol_deleted_event);
}

void OrderBookHandler::addOrder(const Order &order) {
    auto it = symbol_to_order_book.find(order.getSymbolID());
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    OrderBook *book = it->second.get();
    book->addOrder(order);
}

void OrderBookHandler::deleteOrder(uint32_t symbol_id, uint64_t order_id) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    OrderBook *book = it->second.get();
    book->deleteOrder(order_id);
}

void OrderBookHandler::cancelOrder(uint32_t symbol_id, uint64_t order_id, uint64_t cancelled_quantity) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    OrderBook *book = it->second.get();
    book->cancelOrder(order_id, cancelled_quantity);
}

void OrderBookHandler::modifyOrder(uint32_t symbol_id, uint64_t order_id, uint64_t new_order_id, uint64_t new_price) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    OrderBook *book = it->second.get();
    book->modifyOrder(order_id, new_order_id, new_price);
}

void OrderBookHandler::executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity, uint64_t price) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    OrderBook *book = it->second.get();
    book->executeOrder(order_id, quantity, price);
}

void OrderBookHandler::executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity) {
    auto it = symbol_to_order_book.find(symbol_id);
    if (it == symbol_to_order_book.end()) {
        throw std::runtime_error("Symbol does not exist in the book");
    }
    OrderBook *book = it->second.get();
    book->executeOrder(order_id, quantity);
}


}
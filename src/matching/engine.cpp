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

std::string OrderBookHandler::toString() {
    std::ostringstream oss;

    for (const auto& [symbol_id, book_ptr] : symbol_to_order_book) {
        if (book_ptr) {
            oss << "Symbol ID: " << symbol_id << "\n";
            oss << book_ptr->toString() << "\n";
        } else {
            oss << "Symbol ID: " << symbol_id << " has a null OrderBook.\n";
        }
    }

    return oss.str();
}

// constructor 
Engine::Engine(std::unique_ptr<EventHandler> event_handler): orderbook_handler(std::make_unique<OrderBookHandler>(std::move(event_handler))) {}

void Engine::addSymbol(uint32_t symbol_id, const std::string &symbol_name) {
    symbol_id_to_symbol[symbol_id] = std::make_unique<Symbol>(symbol_id, symbol_name);
    orderbook_handler->addOrderBook(symbol_id, symbol_name);
}

void Engine::deleteSymbol(uint32_t symbol_id) {
    auto it = symbol_id_to_symbol.find(symbol_id);
    if (it != symbol_id_to_symbol.end()) {
        orderbook_handler->deleteOrderBook(symbol_id, it->second->name);
        symbol_id_to_symbol.erase(it);
    }
}

bool Engine::hasSymbol(uint32_t symbol_id) const {
    return symbol_id_to_symbol.count(symbol_id) > 0;
}

void Engine::addOrder(const Order &order) {
    orderbook_handler->addOrder(order);
}

void Engine::deleteOrder(uint32_t symbol_id, uint64_t order_id) {
    orderbook_handler->deleteOrder(symbol_id, order_id);
}

void Engine::cancelOrder(uint32_t symbol_id, uint64_t order_id, uint64_t cancelled_quantity) {
    orderbook_handler->cancelOrder(symbol_id, order_id, cancelled_quantity);
}

void Engine::replaceOrder(uint32_t symbol_id, uint64_t order_id, uint64_t new_order_id, uint64_t new_price) {
    orderbook_handler->replaceOrder(symbol_id, order_id, new_order_id, new_price);
}

void Engine::executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity, uint64_t price) {
    orderbook_handler->executeOrder(symbol_id, order_id, quantity, price);
}

void Engine::executeOrder(uint32_t symbol_id, uint64_t order_id, uint64_t quantity) {
    orderbook_handler->executeOrder(symbol_id, order_id, quantity);
}

std::String Engine::toString() const {
    return orderbook_handler->toString();
}

std::ostream &operator<<(std::ostream &os, const Engine &engine) {
    os << engine.orderbook_handler->toString();
    return os;
}

void Engine::exportEngine(const std::string &name) const {
    std::ofstream file(name);
    file << *this;
    file.close();
}
}
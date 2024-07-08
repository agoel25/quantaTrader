#ifndef QUANTA_TRADER_ORDER_BOOK_H
#define QUANTA_TRADER_ORDER_BOOK_H
#include "order.h"

namespace QuantaTrader {

class OrderBook {
public:
    // Adds an order to the order book
    virtual void addOrder(const Order& order) = 0;
    
    // Removes an order from the order book
    virtual void deleteOrder(uint64_t order_id) = 0;
    
    // Modifies an existing order in the order book
    virtual void modifyOrder(uint64_t order_id, uint64_t new_price, uint64_t new_quantity) = 0;

    // Cancels the given property of an order in the book
    virtual void cancelOrder(uint64_t order_id, uint64_t quantity) = 0;

    // Executes an order in the order book
    virtual void executeOrder(uint64_t order_id, uint64_t quantity, uint64_t price) = 0;

    // Executes an order in the order book
    virtual void executeOrder(uint64_t order_id, uint64_t quantity) = 0;

    // Retrieves the best bid order
    virtual Order getBestBid() const = 0;

    // Retrieves the best ask order
    virtual Order getBestAsk() const = 0;

    // Last traded price if any trades have occurred
    virtual uint64_t lastTradedPrice() const = 0;

    // Retrieves a snapshot of the order book
    virtual std::vector<Order> getOrderBookSnapshot() const = 0;

    // Whether the book has this order
    virtual bool hasOrder(uint64_t order_id) const = 0;

    // Gets an order from the book
    virtual const Order &getOrder(uint64_t order_id) const = 0;

    // Whether the book is empty or not
    virtual bool empty() const = 0;

    // Clears the order book
    virtual void clear() = 0;

    // Validates an order
    virtual bool validateOrder(const Order& order) const = 0;

    // Exports the book to a specified path in txt format
    virtual void dump(const std::string &path) const = 0;

    virtual std::string toString() const = 0;
    virtual ~OrderBook() = default;
};
}

#endif // QUANTA_TRADER_ORDER_BOOK_H
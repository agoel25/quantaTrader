#ifndef QUANTA_TRADER_ORDER_BOOK_H
#define QUANTA_TRADER_ORDER_BOOK_H
#include "order.h"

namespace QuantaTrader {

class OrderBook {
public:
    // return the symbol id of the book
    virtual uint32_t getSymbolId() const = 0;

    // Adds an order to the order book
    virtual void addOrder(Order order) = 0;
    
    // Removes an order from the order book
    virtual void deleteOrder(uint64_t order_id) = 0;
    
    // Modifies an existing order in the order book
    virtual void modifyOrder(uint64_t order_id, uint64_t new_order_id, uint64_t new_price) = 0;

    // Cancels the given property of an order in the book
    virtual void cancelOrder(uint64_t order_id, uint64_t quantity) = 0;

    // Executes an order in the order book
    virtual void executeOrder(uint64_t order_id, uint64_t quantity, uint64_t price) = 0;

    // Executes an order in the order book
    virtual void executeOrder(uint64_t order_id, uint64_t quantity) = 0;

    // Retrieves the best buy order
    virtual uint64_t getBestBuy() const = 0;

    // Retrieves the best sell order
    virtual uint64_t getBestSell() const = 0;

    // Last traded price if any trades have occurred
    virtual uint64_t lastTradedPrice() const = 0;

    // Whether the book has this order
    virtual bool hasOrder(uint64_t order_id) const = 0;

    // Gets an order from the book
    virtual const Order &getOrder(uint64_t order_id) const = 0;

    // Whether the book is empty or not
    virtual bool empty() const = 0;

    // Exports the book to a specified path in txt format
    virtual void exportOrderBook(const std::string &path) const = 0;

    virtual std::string toString() const = 0;
    virtual ~OrderBook() = default;
};
}

#endif // QUANTA_TRADER_ORDER_BOOK_H
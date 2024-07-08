#ifndef QUANTA_TRADER_PRICE_LEVEL_ORDERBOOK_H
#define QUANTA_TRADER_PRICE_LEVEL_ORDERBOOK_H
#include <map>
#include <limits>
#include "level.h"
#include "orderbook.h"
#include "order.h"
#include "event_handler.h"

namespace QuantaTrader {

struct OrderWithLevelIterator {
    Order order;
    std::map<uint64_t, Level>::iterator level_it;
};

class PriceLevelOrderBook : public OrderBook {
public:
    PriceLevelOrderBook(uint32_t symbol_id, EventHandler &event_handler);

    void addOrder(Order order) override;

    void deleteOrder(uint64_t order_id) override;

    void modifyOrder(uint64_t order_id, uint64_t new_price, uint64_t new_quantity) override;

    void cancelOrder(uint64_t order_id, uint64_t quantity) override;

    void executeOrder(uint64_t order_id, uint64_t quantity, uint64_t price) override;

    void executeOrder(uint64_t order_id, uint64_t quantity) override;

    uint64_t getBestBid() const override {
        // if there are no bid levels, return 0
        if (bid_levels.empty()) {
            return 0;
        }
        // else return the highest bid level
        return bid_levels.rbegin()->first;
    }

    uint64_t getBestAsk() const override {
        // if there are no ask levels, return max int
        if (ask_levels.empty()) {
            return std::numeric_limits<uint64_t>::max();
        }
        // else return the lowest ask level
        return ask_levels.begin()->first;
    }

    uint64_t lastTradedPrice() const override {
        return last_traded_price;
    }

    bool hasOrder(uint64_t order_id) const override {
        return orders.find(order_id) != orders.end();
    }

    const Order &getOrder(uint64_t order_id) const override {
        return orders.find(order_id)->second.order;
    }

    bool empty() const override {
        return orders.empty();
    }

    void dump(const std::string &path) const override;

    std::string toString() const override;
    friend std::ostream &operator<<(std::ostream &os, const PriceLevelOrderBook &book);

protected:
    void deleteOrder(uint64_t order_id, bool notification) const override;

    void addMarketOrder(Order &order);

    // helper function for addMarketOrder
    void insertMarketOrder(const Order &order);

    void addLimitOrder(Order &order);

    // helper function for addLimitOrder
    void insertLimitOrder(const Order &order);

    void addStopOrder(Order &order);

    // helper function for addStopOrder
    void insertStopOrder(const Order &order);

    void insertTrailingStopOrder(const Order &order);

    uint64_t calculateStopPrice(Order &order);

};
}

#endif // QUANTA_TRADER_PRICE_LEVEL_ORDERBOOK_H
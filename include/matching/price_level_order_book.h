#ifndef QUANTA_TRADER_PRICE_LEVEL_ORDER_BOOK_H
#define QUANTA_TRADER_PRICE_LEVEL_ORDER_BOOK_H
#include <map>
#include <limits>
#include "level.h"
#include "order_book.h"
#include "robin_hood.h"
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

    uint32_t getSymbolID() const override {
        return symbol_id;
    }

    void addOrder(Order order) override;

    void deleteOrder(uint64_t order_id) override;

    void modifyOrder(uint64_t order_id, uint64_t new_order_id, uint64_t new_price) override;

    void cancelOrder(uint64_t order_id, uint64_t quantity) override;

    void executeOrder(uint64_t order_id, uint64_t quantity, uint64_t price) override;

    void executeOrder(uint64_t order_id, uint64_t quantity) override;

    uint64_t getBestBuy() const override {
        // if there are no buy levels, return 0
        if (buy_levels.empty()) {
            return 0;
        }
        // else return the highest buy level
        return buy_levels.rbegin()->first;
    }

    uint64_t getBestSell() const override {
        // if there are no sell levels, return max int
        if (sell_levels.empty()) {
            return std::numeric_limits<uint64_t>::max();
        }
        // else return the lowest sell level
        return sell_levels.begin()->first;
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

    void exportOrderBook(const std::string &path) const override;

    std::string toString() const override;
    friend std::ostream &operator<<(std::ostream &os, const PriceLevelOrderBook &book);

protected:
    void deleteOrder(uint64_t order_id, bool notification) const override;

    void addMarketOrder(Order &order);

    void addLimitOrder(Order &order);

    // helper function for addLimitOrder
    void insertLimitOrder(const Order &order);

    void addStopOrder(Order &order);

    // helper function for addStopOrder
    void insertStopOrder(const Order &order);

    void insertTrailingStopOrder(const Order &order);

    // calculates and sets the stop price of a trailing stop order
    uint64_t calculateStopPrice(Order &order);

    // updates the price of trailing stop buy order
    void updateBuyStopOrders();

    // updates the price of trailing stop sell order
    void updateSellStopOrders();

    // activates stop limit and restart market orders if the last traded price is suitable.
    void activateStopOrders();

    // helper function for activateStopOrders for the Buy side
    bool activateBuyStopOrders();

    // helper function for activateStopOrders for the Sell side
    bool activateSellStopOrders();

    // helper function for activateBuyStopOrders and activateSellStopOrders
    void activateStopOrder(Order order);

    void match(Order &order);

    // helper function for match order
    [[nodiscard]] bool canMatchOrder(const Order &order) const;

    // matches 2 orders at a particular price
    void executeOrders(Order &sell, Order &buy, uint64_t executing_price);

    // returns the last traded buy price
    uint64_t lastTradedBuyPrice() const {
        return last_traded_price;
    }

    uint64_t lastTradedSellPrice() const {
        // if no trades have been made return max int
        if (last_traded_price == 0) {
            return std::numeric_limits<uint64_t>::max();
        }
        // else return last_traded_price
        return last_traded_price;
    }

    // symbol ID of the book
    uint32_t symbol_id;

    EventHandler &event_handler;

    // current price of the symbol
    uint64_t last_traded_price;

    uint64_t trailing_buy_price;
    uint64_t trailing_sell_price;

    // using robin_hood unordered_map : https://github.com/martinus/robin-hood-hashing
    // for availability of reverse traversal of the map, and better performance
    // orderID: OrderWithLevelIterator
    robin_hood::unordered_map<uint64_t, OrderWithLevelIterator> orders;

    // buy levels are sorted in ascending order
    // sell levels are sorted in descending order
    // price : levels
    std::map<uint64_t, Level> sell_levels;
    std::map<uint64_t, Level> buy_levels;

    // price : stop levels
    std::map<uint64_t, Level> stop_sell_levels;
    std::map<uint64_t, Level> stop_buy_levels;
    
    // price : trailing stop levels
    std::map<uint64_t, Level> trailing_stop_sell_levels;
    std::map<uint64_t, Level> trailing_stop_buy_levels;
};
}

#endif // QUANTA_TRADER_PRICE_LEVEL_ORDER_BOOK_H
#ifndef OUANTA_TRADER_LEVEL_H
#define OUANTA_TRADER_LEVEL_H
#include "order.h"

namespace QuantaTrader {

enum class LevelSide {
    SELL = 0,
    BUY = 1
};

using namespace boost::intrusive;

class Level {
public:
    Level(uint64_t price, LevelSide side, uint32_t symbol_id);
    const list<Order> &getOrders() const;
    list<Order> &getOrders();

    inline uint64_t getPrice() const { return price; }
    inline uint64_t getVolume() const { return volume; }
    inline LevelSide getSide() const { return side; }
    inline uint64_t getSymbolId() const { return symbol_id; }
    inline size_t size() const { return orders.size(); }
    inline size_t empty() const { return orders.empty(); }

    Order &front(); // least recently inserted order in the level
    Order &back(); // most recently inserted order in the level
    void removeFront(); // removes the least recently inserted order
    void removeBack(); // removes the most recently inserted order

    void addOrder(Order &order);
    void deleteOrder(const Order &order);
    void reduceVolume(uint64_t amount);

    void popFront(); // removes the oldest order inserted in the level
    void popBack(); // removes the newest order inserted in the level

    std::string toString() const;
    friend std::ostream &operator<<(std::ostream &os, const Level &level);

private:
uint64_t price;
    list<Order> orders;
    LevelSide side;
    uint32_t symbol_id;
    uint64_t volume;
};
}

#endif // OUANTA_TRADER_LEVEL_H
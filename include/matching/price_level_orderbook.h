#ifndef QUANTA_TRADER_PRICE_LEVEL_ORDERBOOK_H
#define QUANTA_TRADER_PRICE_LEVEL_ORDERBOOK_H
#include <map>
#include <limits>
#include "level.h"
#include "orderbook.h"
#include "order.h"

namespace QuantaTrader {

struct OrderWithLevelIterator {
    Order order;
    std::map<uint64_t, Level>::iterator level_it;
};

class PriceLevelOrderBook : public OrderBook {
public:

private:

};
}

#endif // QUANTA_TRADER_PRICE_LEVEL_ORDERBOOK_H
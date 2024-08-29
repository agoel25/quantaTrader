#ifndef QUANTA_TRADER_GENERATE_ORDERS_H
#define QUANTA_TRADER_GENERATE_ORDERS_H

#include <random>
#include <vector>
#include "order.h"

namespace QuantaTrader {
// generate random orders for benchmarking
void generateOrders(std::vector<Order> &orders, uint32_t num_orders, uint32_t num_symbols);
}

#endif

#include "generate_orders.h"

using namespace QuantaTrader;

namespace QuantaTrader {
void generateOrders(std::vector<Order> &orders, uint32_t num_orders, uint32_t num_symbols) {
    std::random_device rd; // create a random device number generator
    std::mt19937 gen(rd()); // set seed for the generator
    std::uniform_int_distribution<uint64_t> order_distribution{1, 10};
    std::uniform_int_distribution<uint64_t> symbol_distribution{1, num_symbols};
    for (uint32_t i = 1; i <= num_orders; ++i) {
        OrderSide side = OrderSide::SELL;
        if (i % 2 == 0) {
            side = OrderSide::BUY;
        }
        OrderTimeInForce time_inf_force = OrderTimeInForce::GTC;
        uint64_t base_price = 100;
        if (side == OrderSide::SELL) {
            base_price = 105;
        };
        uint64_t price = order_distribution(gen) + base_price; // add variability to price by adding random num
        uint64_t quantity = (order_distribution(gen)) * 100; // random order quantity 
        uint64_t order_id = i;
        uint64_t symbol_id = symbol_distribution(gen); // random symbol values

        // add orders to the orders vector
        if (side == OrderSide::SELL) {
            orders.push_back(Order::limitSellOrder(order_id, symbol_id, price, quantity, time_inf_force));
        } else {
            orders.push_back(Order::limitBuyOrder(order_id, symbol_id, price, quantity, time_inf_force));
        }
    }
}
}

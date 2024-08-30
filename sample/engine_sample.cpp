#include <iostream>
#include "engine.h"
#include "event_handler_sample.h"

using namespace QuantaTrader;

int main() {
    // create a new engine with the sample event handler
    auto event_handler = std::unique_ptr<EventHandler>(new EventHandlerSample);
    Engine engine{std::move(event_handler)};

    // add a new symbol to the engine (creates a new order book for this symbol)
    std::string symbol_name = "SMPL";
    uint32_t symbol_id = 1;
    engine.addSymbol(symbol_id, symbol_name);

    // add a limit buy order with higher price than the sell order
    Order order1 = Order::limitBuyOrder(1, symbol_id, 350, 200, OrderTimeInForce::GTC);
    engine.addOrder(order1);

    // add a limit sell order with lower price than the buy order
    Order order2 = Order::limitSellOrder(2, symbol_id, 300, 300, OrderTimeInForce::GTC);
    engine.addOrder(order2);

    // we expect 2 orders to be added, 2 orders to execute, and then the buy order to get deleted since its quantity is lesser
    // changing the price and order values will result in different output
    return 0;
}

#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>
#include <memory>
#include "generate_orders.h"
#include "engine.h"
#include "event_handler.h"

using namespace QuantaTrader;

static void Benchmark(benchmark::State &state) {
    const uint64_t num_symbols = state.range(0);
    const uint64_t num_orders = state.range(1);
    std::vector<Order> orders;
    orders.reserve(num_orders);
    generateOrders(orders, num_orders, num_symbols);

    for (auto i : state) {
        // pause timing during engine setup
        state.PauseTiming();
        Engine engine{std::make_unique<EventHandler>()};

        // add num_symbols number of symbols to the engine
        for (uint32_t i = 1; i <= num_symbols; ++i) {
            engine.addSymbol(i, "BNCH");
        }
        // resume timing now that setup is complete
        state.ResumeTiming();

        // add all orders and measure time
        for (const auto &order : orders) {
            engine.addOrder(order);
        }
    }
}

BENCHMARK(Benchmark)
    ->Unit(benchmark::kMillisecond)
    ->Args({1, 100000})
    ->Args({1, 200000})
    ->Args({1, 300000})
    ->Args({1, 400000})
    ->ArgNames({"symbols", "orders"})
    ->Iterations(1);

BENCHMARK(Benchmark)
    ->Unit(benchmark::kMillisecond)
    ->Args({100, 100000})
    ->Args({100, 200000})
    ->Args({100, 300000})
    ->Args({100, 400000})
    ->ArgNames({"symbols", "orders"})
    ->Iterations(1);

BENCHMARK(Benchmark)
    ->Unit(benchmark::kMillisecond)
    ->Args({1000, 100000})
    ->Args({1000, 200000})
    ->Args({1000, 300000})
    ->Args({1000, 400000})
    ->ArgNames({"symbols", "orders"})
    ->Iterations(1);

BENCHMARK(Benchmark)
    ->Unit(benchmark::kMillisecond)
    ->Args({2600, 100000})
    ->Args({2600, 200000})
    ->Args({2600, 300000})
    ->Args({2600, 400000})
    ->Args({2600, 500000})
    ->ArgNames({"symbols", "orders"})
    ->Iterations(1);

BENCHMARK_MAIN();

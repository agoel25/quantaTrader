# quantaTrader
**quantaTrader** is a highly optimized, low-latency order matching system designed for high-frequency trading (HFT) applications. Written in C++, it is capable of handling millions of orders per second with very low latency.


## Latency
The primary focus of quantaTrader is low latency. High-frequency trading typically requires microsecond (10^-6 seconds) to sub-microsecond latency per operation to maintain competitiveness in the market. The engine achieves this by employing various optimization techniques.

Current benchmarking on an M2 MacBook Pro shows an average per operation speed of **1.5 microseconds**.

## CPU and Memory Optimization

1. **Memory Alignment and Cache Optimization**: data structures are aligned in memory with CPU word boundaries that are in powers of 2. This alignment enhances CPU cache efficiency by reducing the number of cache lines needed to access frequently used data, minimizing cache misses, and improving overall performance.

4. **Minimal Dynamic Memory Allocation**: direct management of objects in containers reduces the overhead associated with frequent dynamic memory operations (like new or delete). Thus reducing overall memory fragmentation and overhead.

5. **Asynchronous I/O and Event Handling**: when orders are added, modified, or executed, the corresponding I/O event is handled asynchronously. This ensures the main processing thread is not stalled by I/O operations, thereby reducing waiting times.

6. **[Robin Hood Hashing for Efficient Lookup](https://github.com/martinus/robin-hood-hashing)**: Robin Hood hashing is used in in large hash maps to minimize variance in probe lengths, thus ensuring a more uniform distribution of entries. Leads to a much better lookup performance and cache efficiency.

3. **Use of Structured Bindings (auto [a, b])**: this feature from C++17 allows efficient unpacking of tuples returned from functions like emplace(). This minimizes the overhead associated with copying or accessing elements separately.

7. **Minimized Copy Operations with std::piecewise_construct**: reduces the number of copies required for constructing elements in maps. This avoids unnecessary copying and enhances performance by directly constructing the elements in place.

## System Structure
There are 4 primary components in this system:
1. **Order**: Represents an individual trading order, with various attributes like price, quantity, symbol, type, etc. Supports order types like market orders, limit orders, stop orders, and trailing stop orders, each with specialized handling functions.
2. **Level**: Represents a collection of orders at a specific price level within the order book. It manages all orders that share the same price and order side, sorted by their entry time (FIFO ordering).
2. **Order Book**: Each symbol has its own order book that manages all buy and sell levels for that symbol. This has all the complicated logic related to adding, matching, executing, deleting orders.
3. **Engine**: The central component that orchestrates interactions between various order books and manages global trading operations. Has a separate order book for each symbol: 1000 symbols in the trading engine means 1000 order books.

Sample Hierarchy:
```
Engine
│
├── OrderBook (Symbol: SMPL)
│   ├── BUY Levels
│   │   ├── Level (Price: 150.00)
│   │   │   ├── Order
│   │   │   └── Order
│   │   └── Level (Price: 148.00)
│   │       ├── Order
│   │       └── Order
│   │
│   └── SELL Levels
│       ├── Level (Price: 151.00)
│       │   ├── Order
│       │   └── Order
│       └── Level (Price: 153.00)
│           └── Order
│
├── OrderBook (Symbol: EXMP)
│   ├── BUY Levels
│   │   └── Level (Price: 248.00)
│   │       └── Order
│   │
│   └── SELL Levels
│       └── Level (Price: 252.00)
│           ├── Order
│           └── Order

```
*Note: for simplicity, details and fields have been truncated. Information in the diagram is not complete*

## Get Started ## 

### Prerequisites
- C++17 or higher
- CMake
- [Boost Libraries](https://www.boost.org/)
- [Google Benchmark](https://github.com/google/benchmark)

### Installation
1. Clone the repository
    ```
    git clone https://github.com/agoel25/quantaTrader.git
    cd quantaTrader
    ```
2. Build the Project: Use the provided build.sh script to automate the build process
    ```
    ./build.sh
    ```
    The script will create a build directory, run CMake to configure the project, and compile the source files. You will also be able to see the benchmark results if you have google benchmark installed.
3. Test with Sample Executable: Use the sample executable to test basic functionality
    ```
    ./build/engine_sample
    ```

### Benchmarking Results
**Legend: Last entry gives the result of adding/matching 500,000 orders for 2600 symbols**
![](./resources/benchmark.png)

### References
1. 
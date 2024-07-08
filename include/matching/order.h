#ifndef QUANTA_TRADER_ORDER_H
#define QUANTA_TRADER_ORDER_H

#include <cstdint>
#include <chrono>

enum class OrderSide : uint8_t {
    BUY = 0,
    SELL = 1
};

enum class OrderType : uint8_t {
    MARKET = 0, // buy or sell immediately at the best available price
    LIMIT = 1, // buy or sell at a specific price or better. not guaranteed to execute 
    STOP = 2, // buy or sell once the price reaches a specified level
    STOP_LIMIT = 3, // buy or sell once the price reaches a specified stop level, 
                    // then execute a limit order at a specified price
    TRAILING_STOP = 4, // a stop order that moves with the market price to lock in profits
    TRAILING_STOP_LIMIT = 5 // a trailing stop order that triggers a limit order instead of 
                            // a market order when the stop level is reached.
};

enum class OrderTimeInForce : uint8_t {
    GTC = 0,
    IOC = 1,
    AON = 2,
    FOK = 3
};

class Order {
public:
    // Factory method to create instances
    static Order createOrder(uint64_t id, OrderType type, OrderSide side, OrderTimeInForce time_in_force, uint32_t symbol_id, uint64_t price, uint64_t stop_price, 
                             uint64_t quantity, uint64_t executed_quantity, uint64_t open_quantity, std::chrono::time_point<std::chrono::high_resolution_clock> timestamp) {
        return Order(id, type, side, time_in_force, symbol_id, price, stop_price, quantity, executed_quantity, open_quantity, timestamp);
    }

    inline uint64_t getId() const { return id; }
    inline OrderType getType() const { return type; }
    inline OrderSide getSide() const { return side; }
    inline OrderTimeInForce getTimeInForce() const { return time_in_force; }
    inline uint32_t getSymbolId() const { return symbol_id; }
    inline uint64_t getPrice() const { return price; }
    inline uint64_t getStopPrice() const { return stop_price; }
    inline uint64_t getQuantity() const { return quantity; }
    inline uint64_t getExecutedQuantity() const { return executed_quantity; }
    inline uint64_t getOpenQuantity() const { return open_quantity; }
    inline std::chrono::time_point<std::chrono::high_resolution_clock> getTimestamp() const { return timestamp; }

    bool operator==(const Order &other) const
    {
        return id == other.id;
    }

    bool operator!=(const Order &other) const
    {
        return !(*this == other);
    }

private:
    Order(uint64_t id, OrderType type, OrderSide side, OrderTimeInForce time_in_force, uint32_t symbol_id, uint64_t price, uint64_t stop_price, 
        uint64_t quantity, uint64_t executed_quantity, uint64_t open_quantity, std::chrono::time_point<std::chrono::high_resolution_clock> timestamp);

    inline void setId(uint64_t id) { this->id = id; }
    inline void setType(OrderType type) { this->type = type; }
    inline void setSide(OrderSide side) { this->side = side; }
    inline void setTimeInForce(OrderTimeInForce time_in_force) { this->time_in_force = time_in_force; }
    inline void setSymbolId(uint32_t symbol_id) { this->symbol_id = symbol_id; }
    inline void setPrice(uint64_t price) { this->price = price; }
    inline void setStopPrice(uint64_t stop_price) { this->stop_price = stop_price; }
    inline void setQuantity(uint64_t quantity) { this->quantity = quantity; }
    inline void setExecutedQuantity(uint64_t executed_quantity) { this->executed_quantity = executed_quantity; }
    inline void setOpenQuantity(uint64_t open_quantity) { this->open_quantity = open_quantity; }
    inline void setTimestamp(std::chrono::time_point<std::chrono::high_resolution_clock> timestamp) { this->timestamp = timestamp; }

    uint64_t id;  // Unique identifier for the order
    OrderType type;  // Type of the order
    OrderSide side;  // Side of the order
    OrderTimeInForce time_in_force;  // Time in force for the order
    uint32_t symbol_id;  // Symbol identifier
    uint64_t price;  // Price
    uint64_t stop_price;  // Stop price
    uint64_t quantity;  // Quantity of the order
    uint64_t executed_quantity;  // Executed quantity
    uint64_t open_quantity;  // Open quantity
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;  // Time the order was placed
};

#endif // QUANTA_TRADER_ORDER_H
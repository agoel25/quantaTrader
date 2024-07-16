#ifndef QUANTA_TRADER_ORDER_H
#define QUANTA_TRADER_ORDER_H

#include <cstdint>
#include <chrono>
#include <boost/intrusive/list.hpp>

namespace QuantaTrader {

using namespace boost::intrusive;
class PriceLevelOrderBook;
class Level;

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

struct Order : public list_base_hook<> {
public:
    static Order marketBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t quantity, OrderTimeInForce time_in_force);
    static Order marketSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t quantity, OrderTimeInForce time_in_force);

    static Order limitBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t quantity, OrderTimeInForce time_in_force);
    static Order limitSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t quantity, OrderTimeInForce time_in_force);

    static Order stopBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force);
    static Order stopAskOrder(uint64_t order_id, uint32_t symbol_id, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force);

    static Order stopLimitBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force);
    static Order stopLimitSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t stop_price, uint64_t quantity, OrderTimeInForce time_in_force);

    static Order trailingStopBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force);
    static Order trailingStopSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force);

    static Order trailingStopLimitBuyOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force);
    static Order trailingStopLimitSellOrder(uint64_t order_id, uint32_t symbol_id, uint64_t price, uint64_t trail_amount, uint64_t quantity, OrderTimeInForce time_in_force);

    inline uint64_t getId() const { return id; }
    inline OrderType getType() const { return type; }
    inline OrderSide getSide() const { return side; }
    inline OrderTimeInForce getTimeInForce() const { return time_in_force; }
    inline uint32_t getSymbolId() const { return symbol_id; }
    inline uint64_t getPrice() const { return price; }
    inline uint64_t getStopPrice() const { return stop_price; }
    inline uint64_t getLastExecutedPrice() const { return last_executed_price; }
    inline uint64_t getTrailAmount() const { return trail_amount; }
    inline uint64_t getQuantity() const { return quantity; }
    inline uint64_t getExecutedQuantity() const { return executed_quantity; }
    inline uint64_t getOpenQuantity() const { return open_quantity; }
    inline uint64_t getLastExecutedQuantity() const { return last_executed_quantity; }
    inline std::chrono::time_point<std::chrono::high_resolution_clock> getTimestamp() const { return timestamp; }

    bool operator==(const Order &other) const
    {
        return id == other.id;
    }

    bool operator!=(const Order &other) const
    {
        return !(*this == other);
    }

    std::string toString() const;
    // declaring friends so the private section can be accessed
    friend std::ostream &operator<<(std::ostream &os, const Order &order);
    friend class PriceLevelOrderBook;
    friend class Level;

private:
    Order(uint64_t id, OrderType type, OrderSide side, OrderTimeInForce time_in_force, uint32_t symbol_id, uint64_t price, uint64_t stop_price, 
        uint64_t trail_amount, uint64_t quantity, std::chrono::time_point<std::chrono::high_resolution_clock> timestamp);

    inline void setId(uint64_t id) { this->id = id; }
    inline void setType(OrderType type) { this->type = type; }
    inline void setTimeInForce(OrderTimeInForce time_in_force) { this->time_in_force = time_in_force; }
    inline void setPrice(uint64_t price) { this->price = price; }
    inline void setStopPrice(uint64_t stop_price) { this->stop_price = stop_price; }
    inline void setTrailAmount(uint64_t trail_amount) { this->trail_amount = trail_amount; }

    void setQuantity(uint64_t quantity_) {
        quantity = std::min(quantity_, open_quantity);
        open_quantity = quantity_;
    }

    void execute(uint32_t price_, uint64_t quantity_) {
        open_quantity -= quantity_;
        executed_quantity += quantity_;
        last_executed_price = price_;
        last_executed_quantity = quantity_;
    }

    uint64_t id;  // Unique identifier for the order
    OrderType type;  // Type of the order
    OrderSide side;  // Side of the order
    OrderTimeInForce time_in_force;  // Time in force for the order
    uint32_t symbol_id;  // Symbol identifier
    uint64_t price;  // Price
    uint64_t stop_price;  // Stop price
    uint64_t trail_amount; // Amount the trailing stop price trails behind the market price
    uint64_t last_executed_price;  // Price at which the last portion of the order was executed
    uint64_t quantity;  // Quantity of the order
    uint64_t executed_quantity;  // Executed quantity
    uint64_t open_quantity;  // Open quantity
    uint64_t last_executed_quantity;  // Quantity of the last portion of the order that was executed
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;  // Time the order was placed
};
}

#endif // QUANTA_TRADER_ORDER_H
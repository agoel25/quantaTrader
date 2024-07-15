#ifndef QUANTA_TRADER_EVENT_H
#define QUANTA_TRADER_EVENT_H
#include "order.h"

namespace QuantaTrader {

struct Event {
    virtual ~Event() = default;
};

struct EngineEvent : public Event {
    uint32_t symbol_id;
    explicit EngineEvent(uint32_t symbol_id) : symbol_id(symbol_id) {}
};

struct SymbolAdded : public EngineEvent {
    std::string name;
    SymbolAdded(uint32_t symbol_id, std::string name) : EngineEvent(symbol_id), name(std::move(name)) {}

    friend std::ostream &operator<<(std::ostream &os, const SymbolAdded &notification);
};

struct SymbolDeleted : public EngineEvent {
    std::string name;
    SymbolDeleted(uint32_t symbol_id, std::string name) : EngineEvent(symbol_id), name(std::move(name)) {}

    friend std::ostream &operator<<(std::ostream &os, const SymbolDeleted &notification);
};

struct OrderEvent : public Event {
    Order order;
    explicit OrderEvent(Order order) : order(std::move(order)) {}
};

struct OrderAdded : public OrderEvent {
    explicit OrderAdded(Order order) : OrderEvent(std::move(order)) {}

    friend std::ostream &operator<<(std::ostream &os, const OrderAdded &notification);
};

struct OrderDeleted : public OrderEvent {
    explicit OrderDeleted(Order order) : OrderEvent(std::move(order)) {}

    friend std::ostream &operator<<(std::ostream &os, const OrderDeleted &notification);
};

struct OrderExecuted : public OrderEvent {
    explicit OrderExecuted(Order order) : OrderEvent(std::move(order)) {}

    friend std::ostream &operator<<(std::ostream &os, const OrderExecuted &notification);
};

struct OrderUpdated : public OrderEvent {
    explicit OrderUpdated(Order order) : OrderEvent(std::move(order)) {}

    friend std::ostream &operator<<(std::ostream &os, const OrderUpdated &notification);
};
}

#endif // QUANTA_TRADER_EVENT_H
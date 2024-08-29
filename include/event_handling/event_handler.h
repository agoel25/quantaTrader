#ifndef QUANTA_TRADER_EVENT_HANDLER_H
#define QUANTA_TRADER_EVENT_HANDLER_H
#include "event.h"

namespace QuantaTrader {

class EventHandler {
public: 
    EventHandler(const EventHandler &) = delete; // Prevent copying
    EventHandler(EventHandler &&) = delete; // Prevent moving
    EventHandler &operator=(const EventHandler &) = delete; // Prevent copy assignment
    EventHandler &operator=(EventHandler &&) = delete; // Prevent move assignment

    EventHandler() = default; // Default constructor
    virtual ~EventHandler() = default; // Virtual destructor for proper cleanup

    friend class PriceLevelOrderBook;

    virtual void handleOrderAdded(const OrderAdded &event) {}
    virtual void handleOrderDeleted(const OrderDeleted &event) {}
    virtual void handleOrderUpdated(const OrderUpdated &event) {}
    virtual void handleOrderExecuted(const OrderExecuted &event) {}
    virtual void handleSymbolAdded(const SymbolAdded &event) {}
    virtual void handleSymbolDeleted(const SymbolDeleted &event) {}
};
}

#endif // QUANTA_TRADER_EVENT_HANDLER_H
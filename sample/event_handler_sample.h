#ifndef QUANTA_TRADER_EVENT_HANDLER_SAMPLE_H
#define QUANTA_TRADER_EVENT_HANDLER_SAMPLE_H
#include "event_handler.h"

using namespace QuantaTrader;

class EventHandlerSample : public EventHandler {

protected:
    void handleOrderAdded(const OrderAdded &notification) override {
        std::cout << notification << std::endl;
    }
    void handleOrderDeleted(const OrderDeleted &notification) override {
        std::cout << notification << std::endl;
    }
    void handleOrderUpdated(const OrderUpdated &notification) override {
        std::cout << notification << std::endl;
    }
    void handleOrderExecuted(const OrderExecuted &notification) override {
        std::cout << notification << std::endl;
    }
    void handleSymbolAdded(const SymbolAdded &notification) override {
        std::cout << notification << std::endl;
    }
    void handleSymbolDeleted(const SymbolDeleted &notification) override {
        std::cout << notification << std::endl;
    }
};
#endif
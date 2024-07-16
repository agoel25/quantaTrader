#include <iostream>
#include "event.h"

namespace QuantaTrader {

// Symbol events
std::ostream &operator<<(std::ostream &os, const SymbolAdded &event) {
    os << "Symbol Added\n" << "Name: " << event.name << "\n" << "ID: " << event.symbol_id << "\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SymbolDeleted &event) {
    os << "Symbol Deleted\n" << "Name: " << event.name << "\n" << "ID: " << event.symbol_id << "\n";
    return os;
}

// Order events
std::ostream &operator<<(std::ostream &os, const OrderAdded &event) {
    os << "Order Added\n" << event.order;
    return os;
}

std::ostream &operator<<(std::ostream &os, const OrderDeleted &event) {
    os << "Order Deleted\n" << event.order;
    return os;
}

std::ostream &operator<<(std::ostream &os, const OrderExecuted &event) {
    os << "Order Executed\n" << event.order;
    return os;
}

std::ostream &operator<<(std::ostream &os, const OrderUpdated &event) {
    os << "Order Updated\n" << event.order;
    return os;
}
}
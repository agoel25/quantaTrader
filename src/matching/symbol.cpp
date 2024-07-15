#include <algorithm>
#include <string>
#include "symbol.h"

namespace QuantaTrader {
Symbol::Symbol(uint32_t id, std::string name) : id(id), name(std::move(name)) {}

std::ostream &operator<<(std::ostream &os, const Symbol &symbol) {
    os << "Symbol: " << symbol.name << "Symbol ID: " << symbol.id << "\n";
    return os;
}
}
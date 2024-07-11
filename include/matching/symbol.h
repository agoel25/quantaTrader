#ifndef QUANTA_TRADER_SYMBOL_H
#define QUANTA_TRADER_SYMBOL_H
#include <iostream>
#include <cstdint>

namespace QuantaTrader {

struct Symbol {
    uint32_t id;
    std::string name;
    Symbol(uint32_t id, std::string name);
    friend std::ostream &operator<<(std::ostream &os, const Symbol &symbol);
};
}

#endif // QUANTA_TRADER_SYMBOL_H
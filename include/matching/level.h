#ifndef OUANTA_TRADER_LEVEL_H
#define OUANTA_TRADER_LEVEL_H
#include "order.h"

enum class LevelSide {
    Buy,
    Sell
};

class Level {
public:
    Level(uint64_t price, LevelSide side, uint32_t symbol_id);

};

#endif // OUANTA_TRADER_LEVEL_H
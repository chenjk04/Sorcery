#ifndef CARD_H
#define CARD_H

#include <string>
#include "Decorator.h"
class Player; // forward declaration
class Card : public Decorator {
    std::string cardName;
    int cost;
    Player* owner;
    Player* other;
    void (*startOfTurnEffect)();
    void (*EndOfTurnEffect)();
    void (*EnemyEnterEffect)();
    void (*AllyEnterEffect)();
    void (*EnemyDieEffect)();
    void (*AllyDieEffect)();
};

#endif

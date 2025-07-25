#ifndef HAND_H
#define HAND_H

#include <vector>
#include "Card.h"

class Hand{
public:
    static constexpr int MAX_SIZE=5;
    Hand();
    bool add(Card*card);
    Card* remove(int idx);
    void discard(int idx);
    const std::vector<Card*>& getCards()const;
private:
    std::vector<Card*> cards;
};

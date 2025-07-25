#ifndef DECK_H
#define DECK_H

#include <vector>
#include <random>
#include <string>
#include "Card.h"

class Deck{
public:
    Deck();
    void loadFromFile(const std::string& filename);
    void shuffle();
    Card* draw();
    bool empty()const;
private:
    std::vector<Card*> cards;
    std::mt19937 rng{std::random_device{}()};
};

#endif 

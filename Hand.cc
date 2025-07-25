#include <iostream>
#include "Hand.h"

Hand::Hand() = default;

bool Hand::add(Card* card) {
    if (!card || (int)cards.size() >= MAX_SIZE) return false;
    cards.push_back(card);
    return true;
}

Card* Hand::remove(int idx) {
    if (idx < 0 || idx >= (int)cards.size()) return nullptr;
    Card* c = cards[idx];
    cards.erase(cards.begin() + idx);
    return c;
}

void Hand::discard(int idx) {
    Card* c = remove(idx);
    if (c) delete c;
}

const std::vector<Card*>& Hand::getCards() const {
    return cards;
}

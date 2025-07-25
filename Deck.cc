#include <fstream>
#include <algorithm>
#include <iostream>
#include <chrono>
#include "Deck.h"
#include "CardInfo.h"

Deck::Deck()
    : rng((unsigned)std::chrono::system_clock::now().time_since_epoch().count())
{}

Deck::~Deck() {
    for (auto c : cards) delete c;
}

void Deck::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Failed to open deck file: " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        cards.push_back(new Card(line));
    }
    std::reverse(cards.begin(), cards.end());
}

void Deck::shuffle() {
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card* Deck::draw() {
    if (cards.empty()) return nullptr;
    Card* c = cards.back(); cards.pop_back();
    return c;
}

bool Deck::empty() const {
    return cards.empty();
}

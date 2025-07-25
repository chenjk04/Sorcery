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

    std::string cardName;
    while (std::getline(in, cardName)) {
        if (cardName.empty()) continue;

        // 1) Grab the type + all stats from your central lookup
        const auto& info = CardInfo::get(cardName);

        // 2) Instantiate the right subclass
        if (info.type == "Minion") {
            cards.push_back(new Minion(
                cardName,
                info.cost,
                info.atk,
                info.hp
            ));
        }
        else if (info.type == "Spell") {
            // here we pass an empty/no‐op effect and no target flag;
            // swap in your real effect map if you have one
            cards.push_back(new Spell(
                cardName,
                info.cost,
                /* effect  */ {},
                /* needsTarget */ false
            ));
        }
        else if (info.type == "Enchantment") {
            cards.push_back(new Enchantment(
                cardName,
                info.cost,
                /* attackModValue */ info.atk,
                /* attackModType  */ Enchantment::ModType::ADD,
                /* defenceModValue*/ info.hp,
                /* defenceModType */ Enchantment::ModType::ADD,
                /* attackModStr   */ "",
                /* defenceModStr  */ "",
                /* grantsAbility  */ false,
                /* grantedAbility */ nullptr
            ));
        }
        else if (info.type == "Ritual") {
            cards.push_back(new Ritual(
                cardName,
                info.cost,
                /* charges         */ info.charges,
                /* activationCost  */ info.actCost,
                /* effect          */ [](Player*){}
            ));
        }
        else {
            std::cerr << "Unknown card type \"" << info.type
                      << "\" for card \"" << cardName << "\"\n";
        }
    }

    // 3) reverse so the first‐line of the file is on top of deck
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

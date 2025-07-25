#include "Player.h"
#include "CardInfo.h"
#include "ascii_graphics.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Player::Player(const std::string& name)
    : name_(name), rng(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())) {}

Player::~Player() {}

void Player::setOpponent(Player* p) {
    opponent = p;
}

void Player::loadDeckFromFile(const std::string& filename) {
    std::ifstream in(filename);
    std::string cardName;
    while (std::getline(in, cardName)) {
        if (cardName.empty()) continue;
        const auto& info = CardInfo::get(cardName);
        if (info.type == "Minion") {
            deck.emplace_back(std::make_unique<Minion>(cardName, info.cost, info.atk, info.hp));
        } else if (info.type == "Spell") {
            deck.emplace_back(std::make_unique<Spell>(cardName, info.cost, [](const State&, Player*, Player*){}, false));
        } else if (info.type == "Enchantment") {
            deck.emplace_back(std::make_unique<Enchantment>(cardName, info.cost, info.atk,
                Enchantment::ModType::ADD, info.hp, Enchantment::ModType::ADD,
                "", "", false, nullptr));
        } else if (info.type == "Ritual") {
            deck.emplace_back(std::make_unique<Ritual>(cardName, info.cost, info.charges, info.actCost, []{}));
        }
    }
    std::reverse(deck.begin(), deck.end());
}

void Player::shuffleDeck() {
    std::shuffle(deck.begin(), deck.end(), rng);
}

void Player::drawCard() {
    if (!deck.empty() && hand.size() < 5) {
        hand.push_back(std::move(deck.back()));
        deck.pop_back();
    }
}

void Player::printHand() const {
    std::vector<card_template_t> templates;
    for (const auto& c : hand) {
        const auto& info = CardInfo::get(c->getName());
        if (info.type == "Minion") {
            if (info.actCost > 0) {
                templates.push_back(display_minion_activated_ability(info.name, info.cost, info.atk, info.hp, info.actCost, info.desc));
            } else if (!info.desc.empty()) {
                templates.push_back(display_minion_triggered_ability(info.name, info.cost, info.atk, info.hp, info.desc));
            } else {
                templates.push_back(display_minion_no_ability(info.name, info.cost, info.atk, info.hp));
            }
        } else if (info.type == "Spell") {
            templates.push_back(display_spell(info.name, info.cost, info.desc));
        } else if (info.type == "Ritual") {
            templates.push_back(display_ritual(info.name, info.cost, info.actCost, info.desc, info.charges));
        } else {
            if (info.atk != 0 || info.hp != 0) {
                std::string a = (info.atk > 0 ? "+" : "") + std::to_string(info.atk);
                std::string h = (info.hp > 0 ? "+" : "") + std::to_string(info.hp);
                templates.push_back(display_enchantment_attack_defence(info.name, info.cost, info.desc, a, h));
            } else {
                templates.push_back(display_enchantment(info.name, info.cost, info.desc));
            }
        }
    }
    for (size_t row = 0; row < templates[0].size(); ++row) {
        for (const auto& card : templates) {
            std::cout << card[row];
        }
        std::cout << '\n';
    }
}

void Player::playCard(int handIndex, const State& state) {
    if (handIndex < 0 || handIndex >= static_cast<int>(hand.size())) return;
    auto card = std::move(hand[handIndex]);
    hand.erase(hand.begin() + handIndex);

    if (card->getType() == "Ritual") {
        ritual = std::move(card);  // <- Only store it, do not execute
    } else if (card->getType() == "Minion") {
        board.push_back(std::move(card));
    } else {
        card->execute(state, this, opponent);
    }
}


int source_to_index(State s) {
    switch (s.source) {
        case Source::B1: return 0;
        case Source::B2: return 1;
        case Source::B3: return 2;
        case Source::B4: return 3;
        case Source::B5: return 4;
        default:
            throw std::invalid_argument("Invalid source for board index");
    }
}

int hand_to_index(State s) {
    switch (s.source) {
        case Source::H1: return 0;
        case Source::H2: return 1;
        case Source::H3: return 2;
        case Source::H4: return 3;
        case Source::H5: return 4;
        default:
            throw std::invalid_argument("Invalid source for hand index");
    }
}

int oppo_to_index(State s) {
    switch (s.target) {
        case Target::P2B1: return 0;
        case Target::P2B2: return 1;
        case Target::P2B3: return 2;
        case Target::P2B4: return 3;
        case Target::P2B5: return 4;
        default:
            throw std::invalid_argument("Invalid target for opponent board index");
    }
}


void Player::attack(State state) {
    int index = source_to_index(state);
    Minion* attacker = dynamic_cast<Minion*>(board.at(index).get());
    if (state.target == Target::player2) {
        attacker->attackPlayer(opponent);
    } else {
        int oppoindex = oppo_to_index(state);
        Minion* defender = dynamic_cast<Minion*>(opponent->board.at(oppoindex).get());
        attacker->attackMinion(defender);
    }
}

void Player::use(State state) {
    if (board.size() <= 5) return;
    Ritual* ritual = dynamic_cast<Ritual*>(board.at(5).get());
    if (ritual && ritual->canActivate()) {
        ritual->execute(state, this, opponent);
    }
}

void Player::notify(State state) {
    if (state.action == Action::attack) {
        attack(state);
    } else if (state.action == Action::play) {
        playCard(hand_to_index(state), state);
    } else {
        use(state);
    }
}

void Player::startOfTurn() {
    magic_ = 3;
    drawCard();
    for (const auto& card : board) {
        card->startOfTurnTrigger();
    }
}

void Player::endOfTurn() {
    for (const auto& card : board) {
        card->endOfTurnTrigger();
    }
}

void Player::setHealth(int h) { health_ = h; }
int Player::getHealth() const { return health_; }
int Player::getMagic() const { return magic_; }
const std::string& Player::getName() const { return name_; }





Card* Player::getRitual() const {
    return ritual ? ritual.get() : nullptr;
}

Card* Player::topGraveyard() const {
    return graveyard.empty() ? nullptr : graveyard.back().get();
}

Minion* Player::getMinion(int index) const {
    if (index < 0 || index >= static_cast<int>(board.size())) return nullptr;
    return dynamic_cast<Minion*>(board[index].get());
}


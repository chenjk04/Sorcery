// ===== Player.cpp =====
#include <iostream>
#include <algorithm>
#include "Player.h"
#include "CardInfo.h"
#include "ascii_graphics.h"

Player::Player(const std::string& name)
    : name_(name)
{}

Player::~Player() {}

void Player::constructDeck(const std::string& deckFile) {
    deck.loadFromFile(deckFile);
}

void Player::shuffleDeck() {
    deck.shuffle();
}

void Player::drawCard() {
    Card* c = deck.draw();
    if (c) {
        if (!hand.add(c)) delete c;
    }
}

void Player::printHand() const {
    const auto& cards = hand.getCards();
    int sz = cards.size();
    if (sz == 0) {
        std::cout << "<empty hand>\n";
        return;
    }

    std::vector<std::vector<std::string>> arts;
    for (auto* c : cards) {
        const auto& info = CardInfo::get(c->getName());
        if (info.type == "Minion") {
            if (info.actCost > 0) {
                arts.push_back(display_minion_activated_ability(
                    info.name, info.cost,
                    info.atk, info.hp,
                    info.actCost, info.desc
                ));
            } else if (!info.desc.empty()) {
                arts.push_back(display_minion_triggered_ability(
                    info.name, info.cost,
                    info.atk, info.hp,
                    info.desc
                ));
            } else {
                arts.push_back(display_minion_no_ability(
                    info.name, info.cost,
                    info.atk, info.hp
                ));
            }
        }
        else if (info.type == "Spell") {
            arts.push_back(display_spell(
                info.name, info.cost,
                info.desc
            ));
        }
        else if (info.type == "Ritual") {
            arts.push_back(display_ritual(
                info.name, info.cost,
                info.actCost, info.desc,
                info.charges
            ));
        }
        else { // Enchantment
            if (info.atk != 0 || info.hp != 0) {
                std::string a = (info.atk>0?"+":"") + std::to_string(info.atk);
                std::string h = (info.hp>0?"+":"") + std::to_string(info.hp);
                arts.push_back(display_enchantment_attack_defence(
                    info.name, info.cost,
                    info.desc,
                    a, h
                ));
            } else {
                arts.push_back(display_enchantment(
                    info.name, info.cost,
                    info.desc
                ));
            }
        }
    }
    int rows = arts[0].size();
    for (int r = 0; r < rows; ++r) {
        for (int i = 0; i < sz; ++i) std::cout << arts[i][r];
        std::cout << "\n";
    }
}

void Player::printBoard(const Player& p1, const Player& p2) {
    std::cout << "\n-- Board State --\n";
    
}


template <typename T>
T pop_front(std::vector<T>& vec) {
    if (!vec.empty()) {
        T item = vec.begin();
        vec.erase(vec.begin());
        return item;
    }
    
}
template <typename T>
T pop_at(std::vector<T>& vec, int i) {
    if (i >= vec.size()) {
        throw std::out_of_range("Index out of bounds");
    }
    T item = vec[i];            // step 1: copy
    vec.erase(vec.begin() + i); // step 2: erase
    return item;                // step 3: return
}

void Player::draw() {
    auto card = pop_front(*board_);
    if (card) hand_->emplace_back(card);
}

void Player::play(State state) {
    int handIndex = hand_to_index(state);
    auto card = pop_at(*hand_,handIndex);
    if (!card) return;
    if (card->getType() == "Minion") {board_->emplace_back(card); return;}
    card->execute(state, this, opponent);
    return;
}

int source_to_index(State s) {
    switch (s.source) {
        case Source::B1: return 0;
        case Source::B2: return 1;
        case Source::B3: return 2;
        case Source::B4: return 3;
        case Source::B5: return 4;
        default:
            throw std::invalid_argument("Source must be B1 to B5");
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
            throw std::invalid_argument("Source must be B1 to B5");
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
            throw std::invalid_argument("Source must be B1 to B5");
    }
}


void Player::setHealth(int h) {
    health_ = h;
}

void Player::attack(State state) {
    int index = source_to_index(state);
    Minion* card = dynamic_cast<Minion*>(board_->at(index).get());
    if (state.target == Target::player2) {card->attackPlayer(opponent); return;}
    int oppoindex = oppo_to_index(state);
    Minion* oppocard = dynamic_cast<Minion*>(opponent->board_->at(oppoindex).get());
    card->attackMinion(oppocard);
}

void Player::startOfTurn() {
    magic_ = 3;
    draw();
    for (const auto& cardPtr : *board_) {
    Card* card = cardPtr.get();
    card->startOfTurnTrigger();}
}

void Player::endOfTurn() {
    for (const auto& cardPtr : *board_) {
    Card* card = cardPtr.get();
    card->endOfTurnTrigger();}
}

void Player::showHand() const {
    hand_->show();
}

void Player::showBoard() const {
    board_->show();
}

// void Player::notifyObserver(State state) {
//     Subject::notify(state);
// }
void Player::use(State state) {
    Ritual* card = dynamic_cast<Ritual*>(board_->at(5).get());
    if (card->canActivate()) {
        card->execute(state, this, opponent);
    }
}

void Player::notify(State state) {
    if (state.action == Action::attack) {attack(state);}
    else if (state.action == Action::play) {play(state);}
    else {use(state);}
}

const std::string& Player::getName() const { return name_; }
int Player::getHealth() const { return health_; }
int Player::getMagic()  const { return magic_; }

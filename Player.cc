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


/*
Player::Player(const std::string& name, const std::string& deckFile)
  : name_(name),
    deck_(std::make_unique<DeckComponent>(deckFile)),
    hand_(std::make_unique<HandComponent>()),
    board_(std::make_unique<BoardComponent>()),
    graveyard_(std::make_unique<GraveyardComponent>())
{
    // Wire up Observer/Subject
    deck_->attach(this);
    attach(hand_.get());
    attach(board_.get());
    attach(graveyard_.get());
}

void Player::draw() {
    auto card = dynamic_cast<DeckComponent*>(deck_.get())->draw();
    if (card) hand_->addCard(std::move(card));
}

void Player::play(int handIndex) {
    auto card = hand_->removeCard(handIndex);
    if (!card) return;
    board_->addCard(std::move(card));
}

void Player::setHealth(int h) {
    health_ = h;
}

void Player::attack(int boardIndex, Player& opponent) {
    auto card = board_->removeCard(boardIndex);
    if (!card) return;
    opponent.health_ -= card->cost;
    if (opponent.health_ < 0) opponent.health_ = 0;
    graveyard_->addCard(std::move(card));
}

void Player::startOfTurn() {
    magic_ = 3;
    draw();
    notifyObserver(State::StartOfTurn);
}

void Player::endOfTurn() {
    notifyObserver(State::EndOfTurn);
}

void Player::showHand() const {
    hand_->show();
}

void Player::showBoard() const {
    board_->show();
}

void Player::notifyObserver(State state) {
    Subject::notify(state);
}

void Player::notify(State state) {
    if (state == State::DrawCard) {
        // no extra logic
    }
}

const std::string& Player::getName() const { return name_; }
int Player::getHealth() const { return health_; }
int Player::getMagic()  const { return magic_; }

*/

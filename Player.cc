// ===== Player.cpp =====
#include "Player.h"
#include <iostream>
#include <algorithm>

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

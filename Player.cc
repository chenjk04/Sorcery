// ===== Player.cpp =====
#include "Player.h"
#include <iostream>
#include <algorithm>
#include "Card.h"

Player::Player(const std::string& name, const std::string& deckFile)
  : name_(name),
    deck_(std::make_unique<std::vector<std::unique_ptr<Card>>>(deckFile)),
    hand_(std::make_unique<std::vector<std::unique_ptr<Card>>>()),
    board_(std::make_unique<std::vector<std::unique_ptr<Card>>>()),
    graveyard_(std::make_unique<std::vector<std::unique_ptr<Card>>>())
{
    // Wire up Observer/Subject
    // deck_->attach(this);
    // attach(hand_.get());
    // attach(board_.get());
    // attach(graveyard_.get());
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

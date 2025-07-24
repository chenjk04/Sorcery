#include "Player.h"
#include "DeckComponent.h"
#include "HandComponent.h"
#include "BoardComponent.h"
#include "GraveyardComponent.h"

Player::Player(const std::string& deckFile)
  : name(deckFile),
    health(20),
    magic(3)
{
    // Create and wire components
    deck = std::make_unique<DeckComponent>(deckFile);
    hand = std::make_unique<HandComponent>();
    board = std::make_unique<BoardComponent>();
    graveyard = std::make_unique<GraveyardComponent>();

    // Player observes deck (to receive Draw notifications)
    deck->attach(this);

    // Player notifies hand/board/graveyard when actions occur
    attach(hand.get());
    attach(board.get());
    attach(graveyard.get());
}

Player::~Player() = default;

void Player::draw() {
    // Request deck to draw; deck will notify(State::DrawCard)
    deck->draw();
}

void Player::discard(int handIndex) {
    // Move from hand to graveyard
    auto card = dynamic_cast<HandComponent*>(hand.get())->removeCard(handIndex);
    graveyard->onNotify(State::DiscardCard, card);
}

void Player::startOfTurnAction() {
    magic = 3;              // reset magic
    draw();                 // draw 1 card
    notifyObserver(State::StartOfTurn);
}

void Player::EndOfTurnAction() {
    notifyObserver(State::EndOfTurn);
}

void Player::notifyObserver(State state) {
    // Broadcast state to components
    Subject::notify(state);
}

void Player::notify(State state) {
    // Handle notifications from components
    switch (state) {
        case State::DrawCard: {
            // deck component told us to draw: get top card and add to hand
            auto card = dynamic_cast<DeckComponent*>(deck.get())->takeTopCard();
            dynamic_cast<HandComponent*>(hand.get())->addCard(card);
            break;
        }
        default:
            break;
    }
}

int Player::getHealth() const { return health; }
void Player::setHealth(int h) { health = h; }
int Player::getMagic() const { return magic; }
void Player::setMagic(int m) { magic = m; }
const std::string& Player::getName() const { return name; }

int Player::getCardCost(int handIndex) const {
    return dynamic_cast<HandComponent*>(hand.get())->getCardCost(handIndex);
}

int Player::getMinionAction(int boardIndex) const {
    return dynamic_cast<BoardComponent*>(board.get())->getMinionAction(boardIndex);
}

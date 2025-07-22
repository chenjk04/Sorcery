#ifndef PLAYER_H
#define PLAYER_H
#include <memory>
#include <string>
#include <vector>

#include "Subject.h"
#include "Observer.h"
#include "State.h"
#include "Component.h"
class Player : public Subject, Observer {
    friend class Card;
    std::string name;
    std::unique_ptr<Component> hand;
    std::unique_ptr<Component> deck;
    std::unique_ptr<Component> board;
    std::unique_ptr<Component> graveyard;
    int health = 20;
    int magic;
    // dont need observer list since it owns what it would notify

    Player(std::string filename = "default.deck");
    ~Player();

public:
    void draw();
    void discard();
    int getHealth();
    void setHealth(int h);
    int getMagic();
    void setMagic(int m);
    void notifyObserver(State state) override; // check if the order is legal (enough magic? action point?)
    void notify(State state) override; // for passing down manual active actions, not passive actions like draw cards at the start of the turn.
    void startOfTurnAction(); // gain magic, draw card, start of turn 
    void EndOfTurnAction();
    int getCardCost(State state);
    int getMinionAction(State state);
};


#endif

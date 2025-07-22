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
    std::unique_ptr<Component> hand;
    std::unique_ptr<Component> deck;
    std::unique_ptr<Component> board;
    std::unique_ptr<Component> graveyard;
    int health;
    int magic;
    // dont need observer list since it owns what it would notify

    Player(std::string filename = "default.deck");
    ~Player();

public:
    void draw();
    void discard();
    notifyObserver(State state) override;
    notify(State state) override;
};


#endif

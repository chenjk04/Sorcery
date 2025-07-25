#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>

#include "Subject.h"
#include "Observer.h"
#include "State.h"
#include "Component.h"

class Player : public Subject, public Observer {
public:
    Player(const std::string& name, const std::string& deckFile);
    ~Player() = default;

    // Core actions
    void draw();
    void play(int handIndex);
    void attack(int boardIndex, Player& opponent);
    void startOfTurn();
    void endOfTurn();

    // Showers for debugging
    void showHand() const;
    void showBoard() const;

    // Observer
    void notifyObserver(State state);
    void notify(State state) override;

    // Accessors
    const std::string& getName() const;
    int getHealth() const;
    int getMagic() const;
    void setHealth(int );

private:
    std::string name_;
    std::unique_ptr<Component> deck_, hand_, board_, graveyard_;
    int health_{20}, magic_{3};
};

#endif 

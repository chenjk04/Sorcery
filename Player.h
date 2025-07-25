// ===== Player.h =====
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <memory>
#include <random>
#include <chrono>
#include "Card.h"
#include "State.h"

class Player {
    std::string name_;
    int health_ = 20;
    int magic_ = 3;

    std::vector<std::unique_ptr<Card>> deck;
    std::vector<std::unique_ptr<Card>> hand;
    std::vector<std::unique_ptr<Card>> board;
    Player* opponent = nullptr;
    std::default_random_engine rng;

    std::unique_ptr<Card> ritual;
std::vector<std::unique_ptr<Card>> graveyard;

public:
    Player(const std::string& name);
    ~Player();

    void setOpponent(Player* p);

    // In Player.h (public section)
int getId() const;
const std::string& getName() const;
int getHealth() const;
int getMagic() const;

Card* getRitual() const;
Card* topGraveyard() const;
Minion* getMinion(int index) const; // 0-based index


    void loadDeckFromFile(const std::string& filename);
    void shuffleDeck();
    void drawCard();

    


    void printHand() const;
    //void printBoard(const Player& p1, const Player& p2);

    void playCard(int handIndex, const State& state);
    void attack(State state);
    void use(State state);
    void notify(State state);

    void startOfTurn();
    void endOfTurn();

    void setHealth(int h);
    //int getHealth() const;
    //int getMagic() const;
    //const std::string& getName() const;
};


#endif
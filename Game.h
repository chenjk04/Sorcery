#ifndef GAME_H
#define GAME_H
#include <string>
#include "State.h"
#include "Player.h"
#include "Subject.h"
class Game {
    std::unique_ptr<Player> P1;
    std::unique_ptr<Player> P2;
    bool P1Active;
    bool testingMode;


public:
    void notify(std::string order, int from, int to);
    void run();// calls shuffle, draw 5, sethealth, set magic, then game loop
// gameloop: call players startofturnaction, notify(State) based on input, players endofturnaction;

};
#endif

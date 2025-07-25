
#include <iostream>
#include <sstream>
#include <string>
#include "Player.h"

void printHelp() {
}

int main(int argc, char* argv[]) {
    std::string name1, deck1, name2, deck2;
    if (argc >= 3) {
        name1 = argv[1]; deck1 = argv[2];
    } else {
        std::cout << "Enter name for Player 1: "; std::getline(std::cin, name1);
        std::cout << "Enter deck file for Player 1: "; std::getline(std::cin, deck1);
    }
    if (argc >= 5) {
        name2 = argv[3]; deck2 = argv[4];
    } else {
        std::cout << "Enter name for Player 2: "; std::getline(std::cin, name2);
        std::cout << "Enter deck file for Player 2: "; std::getline(std::cin, deck2);
    }

    Player p1(name1, deck1);
    Player p2(name2, deck2);
    Player* cur = &p1;
    Player* opp = &p2;

    // initial draws
    for (int i = 0; i < 4; ++i) {
        p1.draw(); p2.draw();
    }

    printHelp();
    std::string line;
    while (true) {
        std::cout << "";
        std::cout << p1.getName() << " (" << p1.getHealth() << ") | "
                  << p2.getName() << " (" << p2.getHealth() << ")";
        std::cout << cur->getName() << "> ";
        if (!std::getline(std::cin, line)) break;
        std::istringstream ss(line);
        std::string cmd; ss >> cmd;
        if (cmd == "quit") break;
        else if (cmd == "help") printHelp();
        else if (cmd == "draw") cur->draw();
        else if (cmd == "play") {
            int n; if (ss >> n) cur->play(n-1);
        } else if (cmd == "attack") {
            int n; if (ss >> n) cur->attack(n-1, *opp);
        } else if (cmd == "end") {
            cur->endOfTurn(); opp->startOfTurn();
            std::swap(cur, opp);
        } else {
            std::cout << "Unknown command. Type 'help'.";
        }
        // show state
        std::cout << "Hand of " << cur->getName() << ":";
        cur->showHand();
        std::cout << "Board of " << cur->getName() << ":";
        cur->showBoard();
    }

    std::cout << "Game over.";
    return 0;
}


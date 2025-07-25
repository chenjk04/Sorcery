#include <iostream>
#include "Player.h"

int main() {
    Player p1("Alice"), p2("Bob");
    p1.constructDeck("default.deck");
    p2.constructDeck("default.deck");
    p1.shuffleDeck(); p2.shuffleDeck();
    for (int i = 0; i < 5; ++i) { p1.drawCard(); p2.drawCard(); }

    std::cout << p1.getName() << "'s hand:\n";
    p1.printHand();
    std::cout << "\n" << p2.getName() << "'s hand:\n";
    p2.printHand();

   // Player::printBoard(p1, p2);
    return 0;
}

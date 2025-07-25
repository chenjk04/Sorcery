#include "Game.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include "Component.h"

void Game::notify(const std::string& order, int from, int to) {
    // Echo the command for now
    std::cout << "Player " << from+1 << " -> Player " << to+1 << ": " << order << std::endl;
}

void Game::run() {
    // Initialize players
    // Assuming P1 and P2 have been constructed elsewhere or here:
    // shuffle decks
    dynamic_cast<DeckComponent*>(P1->getDeck())->shuffle();
    dynamic_cast<DeckComponent*>(P2->getDeck())->shuffle();
    
    // Draw initial hands (5 cards each)
    for(int i = 0; i < 5; ++i) {
        P1->draw();
        P2->draw();
    }
    
    // Set starting health and magic
    // E.g., health default 20, magic default 3
    
    // Enter game loop
    P1Active = true;
    while (true) {
        Player* current = P1Active ? P1.get() : P2.get();
        Player* opponent = P1Active ? P2.get() : P1.get();
        
        current->startOfTurn();
        
        std::string cmd;
        std::getline(std::cin, cmd);
        if (cmd == "quit") break;
        notify(cmd, P1Active ? 0 : 1, P1Active ? 1 : 0);
        
        current->endOfTurn();
        P1Active = !P1Active;
    }
}


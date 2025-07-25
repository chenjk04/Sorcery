#include <iostream>
#include <string>
#include <sstream>
#include "Player.h"
#include "ascii_graphics.h"
#include "State.h"

static Target fromIndices(int i, int j, int playerId) {
    if (j == -1) return playerId == 1 ? Target::player2 : Target::player1;
    if (playerId == 1) {
        switch(j) {
            case 0: return Target::P2B1;
            case 1: return Target::P2B2;
            case 2: return Target::P2B3;
            case 3: return Target::P2B4;
            case 4: return Target::P2B5;
        }
    } else {
        switch(j) {
            case 0: return Target::P1B1;
            case 1: return Target::P1B2;
            case 2: return Target::P1B3;
            case 3: return Target::P1B4;
            case 4: return Target::P1B5;
        }
    }
    return Target::NONE;
}

void printBoard(Player& playerOne, Player& playerTwo) {
    std::cout << EXTERNAL_BORDER_CHAR_TOP_LEFT;
    for (int i = 0; i < 165; ++i) std::cout << EXTERNAL_BORDER_CHAR_LEFT_RIGHT;
    std::cout << EXTERNAL_BORDER_CHAR_TOP_RIGHT << std::endl;

    auto getPlayerLine = [](Player& p) -> std::vector<std::vector<std::string>> {
        std::vector<std::vector<std::string>> line;
        Ritual* ritual = dynamic_cast<Ritual*>(p.getRitual());
        line.emplace_back(ritual ? display_ritual(
            ritual->getName(),
            ritual->getCost(),
            ritual->getChargeCost(),
            ritual->getDescription(),
            ritual->getCharges()) : CARD_TEMPLATE_BORDER);
        line.emplace_back(CARD_TEMPLATE_EMPTY);
        line.emplace_back(display_player_card(p.getId(), p.getName(), p.getHealth(), p.getMagic()));
        line.emplace_back(CARD_TEMPLATE_EMPTY);
        Card* g = p.topGraveyard();
        line.emplace_back(g ? g->render() : CARD_TEMPLATE_BORDER);
        return line;
    };

    auto getMinions = [](Player& p) -> std::vector<std::vector<std::string>> {
        std::vector<std::vector<std::string>> out;
        for (int i = 0; i < 5; ++i) {
            Minion* m = p.getMinion(i);
            out.emplace_back(m ? m->render() : CARD_TEMPLATE_BORDER);
        }
        return out;
    };

    auto row1 = getPlayerLine(playerOne);
    for (size_t i = 0; i < row1[0].size(); ++i) {
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN;
        for (int j = 0; j < 5; ++j) std::cout << row1[j][i];
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN << std::endl;
    }

    auto p1Minions = getMinions(playerOne);
    for (size_t i = 0; i < p1Minions[0].size(); ++i) {
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN;
        for (int j = 0; j < 5; ++j) std::cout << p1Minions[j][i];
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN << std::endl;
    }

    for (const auto& line : CENTRE_GRAPHIC) std::cout << line << std::endl;

    auto p2Minions = getMinions(playerTwo);
    for (size_t i = 0; i < p2Minions[0].size(); ++i) {
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN;
        for (int j = 0; j < 5; ++j) std::cout << p2Minions[j][i];
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN << std::endl;
    }

    auto row2 = getPlayerLine(playerTwo);
    for (size_t i = 0; i < row2[0].size(); ++i) {
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN;
        for (int j = 0; j < 5; ++j) std::cout << row2[j][i];
        std::cout << EXTERNAL_BORDER_CHAR_UP_DOWN << std::endl;
    }

    std::cout << EXTERNAL_BORDER_CHAR_BOTTOM_LEFT;
    for (int i = 0; i < 165; ++i) std::cout << EXTERNAL_BORDER_CHAR_LEFT_RIGHT;
    std::cout << EXTERNAL_BORDER_CHAR_BOTTOM_RIGHT << std::endl;
}

int main() {
    Player p1("Alice");
    Player p2("Bob");
    p1.setOpponent(&p2);
    p2.setOpponent(&p1);

    p1.loadDeckFromFile("default.deck");
    p2.loadDeckFromFile("default.deck");
    p1.shuffleDeck();
    p2.shuffleDeck();

    for (int i = 0; i < 5; ++i) {
        p1.drawCard();
        p2.drawCard();
    }

    Player* active = &p1;
    Player* inactive = &p2;

    std::string cmd;
    State dummyState;
    while (true) {
        printBoard(p1, p2);
        std::cout << "\n" << active->getName() << "'s turn > ";
        std::getline(std::cin, cmd);
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;

        if (action == "quit") break;
        else if (action == "hand") active->printHand();
        else if (action == "play") {
            int index; iss >> index;
            active->playCard(index, dummyState);
            dummyState.action = Action::play;
            dummyState.source = static_cast<Source>(static_cast<int>(Source::B1) + index);
            dummyState.target = active == &p1 ? Target::P1BALL : Target::P2BALL;
            active->activateTrigger(dummyState);
        }
        else if (action == "inspect") {
            int index; iss >> index;
            active->inspect(index);
        }
        else if (action == "trigger") {
            active->activateTrigger(dummyState);
        }
        else if (action == "attack") {
            int i, j;
            iss >> i;
            if (iss >> j) {
                dummyState.target = fromIndices(i, j, active == &p1 ? 1 : 2);
            } else {
                dummyState.target = fromIndices(i, -1, active == &p1 ? 1 : 2);
            }
            dummyState.source = static_cast<Source>(static_cast<int>(Source::B1) + i);

            dummyState.action = Action::attack;
            active->attack(dummyState);
        }
        else if (action == "end") {
            active->endOfTurn();
            std::swap(active, inactive);
            active->startOfTurn();
        }
        else {
            std::cout << "Unknown command. Try: hand, play [i], inspect [i], attack i [j], trigger, end, quit.\n";
        }

        if (p1.getHealth() <= 0) {
            std::cout << "Bob wins!\n";
            break;
        } else if (p2.getHealth() <= 0) {
            std::cout << "Alice wins!\n";
            break;
        }
    }
    return 0;
}

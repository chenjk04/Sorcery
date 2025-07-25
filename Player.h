#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>

#include "Subject.h"
#include "Observer.h"
#include "State.h"
#include "Deck.h"
#include "Hand.h"

class Card;
class Player : public Subject, public Observer {
public:
    //Player(const std::string& name, const std::string& deckFile);
    Player(const std::string& name);
    ~Player() = default;

    // ADDED
    std::vector<std::unique_ptr<Minion>>      board;
    std::unique_ptr<Ritual>                   ritual;
    std::vector<std::unique_ptr<Enchantment>> enchantments;
    std::vector<std::unique_ptr<Card>>        graveyard;

    Ritual*                                    getRitual()        const { return ritual.get(); }
    const std::vector<std::unique_ptr<Minion>>& getBoard()        const { return board;   }
    const std::vector<std::unique_ptr<Enchantment>>& getEnchantments() const { return enchantments; }
    const std::vector<std::unique_ptr<Card>>&   getGraveyard()    const { return graveyard; }

    void constructDeck(const std::string& deckFile);
    void shuffleDeck();
    void drawCard();
    void printHand() const;
    static void printBoard(const Player& p1, const Player& p2);
    

    // Core actions
    void draw();
    void play(State state);
    void use(State state);
    void attack(State state);
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
    std::unique_ptr<std::vector<std::unique_ptr<Card>>> deck_, hand_, board_, graveyard_;
    int health_{20}, magic_{3};
    Deck deck;
    Hand hand;
    Player* opponent;
};

#endif 
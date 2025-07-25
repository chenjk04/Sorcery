#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>

#include "Subject.h"
#include "Observer.h"
#include "State.h"

class Card;
class Player : public Subject, public Observer {
public:
    Player(const std::string& name, const std::string& deckFile);
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
    std::unique_ptr<std::vector<std::unique_ptr<Card>>> deck_, hand_, board_, graveyard_;
    int health_{20}, magic_{3};
};

#endif 

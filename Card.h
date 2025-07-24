#ifndef CARD_H
#define CARD_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "State.h"
class Component;
class Player; // forward declaration
class Card {
protected:
    std::string name;
    int cost;
    Player* owner;

public:
    Card(const std::string& name, int cost);
    virtual ~Card() = default;
    
    // Pure virtual methods
    virtual std::string getType() const = 0;
    virtual std::unique_ptr<Card> clone() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Common getters
    std::string getName() const { return name; }
    int getCost() const { return cost; }
    Player* getOwner() const { return owner; }
    void setOwner(Player* newOwner) { owner = newOwner; }
    
    // Action handling - works with your State system
    virtual bool canExecute(const State& state, Player* player) const;
    virtual void execute(const State& state, Player* player) {}
    
    
    std::function<void(Player*)> startOfTurnTrigger = nullptr;
    std::function<void(Player*)> endOfTurnTrigger = nullptr;
    std::function<void(Player*)> minionEntersTrigger = nullptr;
    std::function<void(Player*)> minionLeavesTrigger = nullptr;
    
    // For your Component system
    virtual bool requiresTarget() const { return false; }
    virtual bool isValidTarget(Target target) const { return false; }
    
    // Display
    virtual void displayCard() const = 0;
};

// Minion class
class Minion : public Card {
private:
    int baseAttack;     // Original stats
    int baseDefence;
    int attack;         // Current stats (after enchantments)
    int defence;
    int actions;
    int maxActions;
    
    // Track enchantments for inspection and removal
    std::vector<std::unique_ptr<Enchantment>> enchantments;
    
public:
    Minion(const std::string& name, int cost, int attack, int defence);
    
    std::string getType() const override { return "Minion"; }
    
    // Combat
    void attackPlayer(Player* target);
    void attackMinion(Minion* target);
    bool canAttack() const { return actions > 0; }
    void useAction() { if (actions > 0) actions--; }
    void resetActions() { actions = maxActions; }
    
    // Stats
    int getAttack() const { return attack; }
    int getDefence() const { return defence; }
    int getActions() const { return actions; }
    int getBaseAttack() const { return baseAttack; }
    int getBaseDefence() const { return baseDefence; }
    void modifyStats(int attackMod, int defenceMod);
    bool isDead() const { return defence <= 0; }
    
    // Enchantment management
    void addEnchantment(std::unique_ptr<Enchantment> enchantment);
    void removeAllEnchantments();  // Called when minion leaves play
    const std::vector<std::unique_ptr<Enchantment>>& getEnchantments() const { return enchantments; }
    
    // For inspect command
    void displayWithEnchantments() const;
    
    // Activated ability - function pointer approach
    std::function<void(const State&, Player*)> activatedAbility = nullptr;
    int abilityCost = 0;
    
    // Works with your State system
    void execute(const State& state, Player* player) override;
    bool canExecute(const State& state, Player* player) const override;
    
    void displayCard() const override;
    std::string getDescription() const override;
    std::unique_ptr<Card> clone() const override;
};

// Spell class
class Spell : public Card {
private:
    std::function<void(const State&, Player*)> spellEffect;
    bool needsTarget;
    
public:
    Spell(const std::string& name, int cost, 
          std::function<void(const State&, Player*)> effect, 
          bool needsTarget = false);
    
    std::string getType() const override { return "Spell"; }
    
    void execute(const State& state, Player* player) override;
    bool requiresTarget() const override { return needsTarget; }
    
    void displayCard() const override;
    std::string getDescription() const override;
    std::unique_ptr<Card> clone() const override;
};

// Enchantment class
class Enchantment : public Card {
public:
    enum class ModType {
        ADD,      // +X
        MULTIPLY, // *X
        SET       // =X (sets to specific value)
    };

private:
    int attackModValue = 0;
    int defenceModValue = 0;
    ModType attackModType = ModType::ADD;
    ModType defenceModType = ModType::ADD;
    std::string attackModStr = "";  // For display ("+2", "*2", "=5", etc.)
    std::string defenceModStr = "";
    std::function<void(Minion*)> enchantEffect = nullptr;
    bool grantsAbility = false;
    std::function<void(const State&, Player*)> grantedAbility = nullptr;

public:
    
    Enchantment(const std::string& name, int cost, 
                int attackModValue = 0, ModType attackModType = ModType::ADD,
                int defenceModValue = 0, ModType defenceModType = ModType::ADD,
                const std::string& attackModStr = "", const std::string& defenceModStr = "");
    
    // Alternative constructor that parses mod strings
    Enchantment(const std::string& name, int cost, 
                const std::string& attackModStr = "", const std::string& defenceModStr = "");
    
    std::string getType() const override { return "Enchantment"; }
    bool requiresTarget() const override { return true; }
    bool isValidTarget(Target target) const override;
    
    void execute(const State& state, Player* player) override;
    
    
    int getAttackModValue() const { return attackModValue; }
    int getDefenceModValue() const { return defenceModValue; }
    ModType getAttackModType() const { return attackModType; }
    ModType getDefenceModType() const { return defenceModType; }
    std::string getAttackModStr() const { return attackModStr; }
    std::string getDefenceModStr() const { return defenceModStr; }
    
    
    bool grantsActivatedAbility() const { return grantsAbility; }
    std::function<void(const State&, Player*)> getGrantedAbility() const { return grantedAbility; }
    
    
    void displayCard() const override;
    std::string getDescription() const override;
    std::unique_ptr<Card> clone() const override;

private:
    
    // Helper method to generate mod string from value and type
    std::string generateModString(int value, ModType type) const;
};

// Ritual class
class Ritual : public Card {
private:
    int charges;
    int activationCost;
    std::function<void(Player*)> ritualEffect;
    
public:
    Ritual(const std::string& name, int cost, int charges, int activationCost,
           std::function<void(Player*)> effect);
    
    std::string getType() const override { return "Ritual"; }
    
    void execute(const State& state, Player* player) override;
    bool canActivate() const { return charges >= activationCost; }
    void activate(Player* player);
    void addCharges(int amount) { charges += amount; }
    
    int getCharges() const { return charges; }
    int getActivationCost() const { return activationCost; }
    
    void displayCard() const override;
    std::string getDescription() const override;
    std::unique_ptr<Card> clone() const override;
};

#endif




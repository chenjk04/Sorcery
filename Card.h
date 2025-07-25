#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "State.h"
#include "ascii_graphics.h"

class Player;

// --------- Base Class ---------
class Card {
protected:
    std::string name;
    int cost;
    Player* owner;
    std::string description;

public:
    Card(const std::string& name, int cost);
    virtual ~Card() = default;

    const std::string& getName() const;
    virtual std::string getType() const = 0;
    virtual card_template_t render() const = 0;

    virtual int getCost() const { return cost; }

    virtual bool canActivate() const { return false; }
    virtual void consumeCharges() {}

    virtual void execute(const State& state, Player* owner, Player* other) {}

    // Trigger function pointers
    std::function<void(Player* owner, Player* opponent)> startOfTurnTrigger = [](Player*, Player*){};
    std::function<void(Player* owner, Player* opponent)> endOfTurnTrigger= [](Player*, Player*){};
    std::function<void(Player* owner, Player* opponent)> anyMinionEnter= [](Player*, Player*){};
    std::function<void(Player* owner, Player* opponent)> myMinionEnter= [](Player*, Player*){};
    std::function<void(Player* owner, Player* opponent)> anyMinionDie= [](Player*, Player*){};
    std::function<void(Player* owner, Player* opponent)> myMinionDie= [](Player*, Player*){};
};

// --------- Minion ---------
class Enchantment; // forward declaration

class Minion : public Card {
    int baseAttack, baseDefence;
    int attack, defence;
    int actions;
    std::function<void(Player*, Player*)> activatedAbility;
    std::vector<std::unique_ptr<Enchantment>> enchantments;

public:
    Minion(const std::string& name, int cost, int attack, int defence);

    void attackPlayer(Player* target);
    void attackMinion(Minion* target);
    void modifyStats(int attackMod, int defenceMod);
    void addEnchantment(std::unique_ptr<Enchantment> enchantment);
    void removeAllEnchantments();
    void useAbility(Player* target) {if (actions > 0){actions--;} else {return;} this->activatedAbility(this->owner, target);}

    int getDefence() const;
    int getAttack() const;

    bool hasTrigger() const;
    

    std::string getType() const override;
    card_template_t render() const override;

    const std::vector<std::unique_ptr<Enchantment>>& getEnchantments() const {
        return enchantments;
    }
};

// --------- Spell ---------
class Spell : public Card {
    std::function<void(const State&, Player*, Player*)> spellEffect;
    bool needsTarget;

public:
    Spell(const std::string& name, int cost,
          std::function<void(const State&, Player*, Player*)> effect,
          bool needsTarget);

    void execute(const State& state, Player* owner, Player* other) override;
    std::string getType() const override;
    card_template_t render() const override;
    bool requiresTarget() const { return needsTarget; }
};

// --------- Enchantment ---------
class Enchantment : public Card {
public:
    enum class ModType { ADD, MULTIPLY, SET };

private:
    int attackModValue, defenceModValue;
    ModType attackModType, defenceModType;
    std::string attackModStr, defenceModStr;
    bool grantsAbility;
    std::function<void(const State&, Player*)> grantedAbility;

public:
    Enchantment(const std::string& name, int cost,
                int attackModValue, ModType attackModType,
                int defenceModValue, ModType defenceModType,
                const std::string& attackModStr, const std::string& defenceModStr,
                bool grantsAbility,
                std::function<void(const State&, Player*)> grantedAbility);

    void execute(const State& state, Player* owner, Player* other) override;
    bool isValidTarget(Target target) const;

    int getAttackModValue() const { return attackModValue; }
    ModType getAttackModType() const { return attackModType; }
    int getDefenceModValue() const { return defenceModValue; }
    ModType getDefenceModType() const { return defenceModType; }
    bool grantsActivatedAbility() const { return grantsAbility; }
    std::function<void(const State&, Player*)> getGrantedAbility() const { return grantedAbility; }

    std::string getType() const override;
    card_template_t render() const override;

private:
    std::string generateModString(int value, ModType type) const;
};

// --------- Ritual ---------
class Ritual : public Card {
    int charges;
    int activationCost;
    

public:
    Ritual(const std::string& name, int cost, int charges, int activationCost
           );

    void execute(const State& state, Player* player, Player* other) override;
    void consumeCharges() override;
    bool canActivate() const override;

    int getCharges() const;
    int getChargeCost() const;
    std::string getDescription() const;

    std::string getType() const override;
    card_template_t render() const override;
};

#endif

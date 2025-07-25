#include "Card.h"

#include <algorithm>
#include <memory>
// i need setHealth() fn for Player class 
// play: call card.execute if you need to play cards other than minion
// also check cost

// ==================== Card Base Class ====================

Card::Card(const std::string& name, int cost) 
    : name(name), cost(cost), owner(nullptr), description("") {}



// ==================== Minion Class ====================

Minion::Minion(const std::string& name, int cost, int attack, int defence)
    : Card(name, cost), baseAttack(attack), baseDefence(defence), 
      attack(attack), defence(defence), actions(0) {}

void Minion::attackPlayer(Player* target) {
    if (!canAttack() || !target) return;
    
    target->setHealth(target->getHealth() - attack);
    useAction();
}

void Minion::attackMinion(Minion* target) {
    if (!canAttack() || !target) return;
    
    // Both minions damage each other simultaneously
    int myAttack = this->attack;
    int targetAttack = target->attack;
    
    this->defence -= targetAttack;
    target->defence -= myAttack;
    
    useAction();
}

void Minion::modifyStats(int attackMod, int defenceMod) {
    attack += attackMod;
    defence += defenceMod;
}

void Minion::addEnchantment(std::unique_ptr<Enchantment> enchantment) {
    if (!enchantment) return;
    
    // Apply enchantment effects
    int attackMod = enchantment->getAttackModValue();
    int defenceMod = enchantment->getDefenceModValue();
    
    switch (enchantment->getAttackModType()) {
        case Enchantment::ModType::ADD:
            attack += attackMod;
            break;
        case Enchantment::ModType::MULTIPLY:
            attack *= attackMod;
            break;
        case Enchantment::ModType::SET:
            attack = attackMod;
            break;
    }
    
    switch (enchantment->getDefenceModType()) {
        case Enchantment::ModType::ADD:
            defence += defenceMod;
            break;
        case Enchantment::ModType::MULTIPLY:
            defence *= defenceMod;
            break;
        case Enchantment::ModType::SET:
            defence = defenceMod;
            break;
    }
    
    // If enchantment grants an ability, replace current ability
    if (enchantment->grantsActivatedAbility()) {
        activatedAbility = enchantment->getGrantedAbility();
    }
    
    enchantments.push_back(std::move(enchantment));
}

void Minion::removeAllEnchantments() {
    // Reset to base stats
    attack = baseAttack;
    defence = baseDefence;
    activatedAbility = nullptr;
    
    enchantments.clear();
}



void Minion::displayCard() const {
    // Display implementation will be handled elsewhere
}

void Minion::displayWithEnchantments() const {
    // Display implementation will be handled elsewhere
}

// ==================== Spell Class ====================

Spell::Spell(const std::string& name, int cost, 
             std::function<void(const State&, Player*, Player*)> effect, 
             bool needsTarget)
    : Card(name, cost), spellEffect(effect), needsTarget(needsTarget) {}

void Spell::execute(const State& state, Player* owner, Player* other){
    
    
    if (spellEffect) {
        spellEffect(state, owner, other);
    }
}

void Spell::displayCard() const {
    // Display implementation will be handled elsewhere
}

// ==================== Enchantment Class ====================

Enchantment::Enchantment(const std::string& name, int cost,
                        int attackModValue, ModType attackModType,
                        int defenceModValue, ModType defenceModType,
                        const std::string& attackModStr, const std::string& defenceModStr,
                        bool grantsAbility, std::function<void(const State&, Player*)> grantedAbility)
    : Card(name, cost), attackModValue(attackModValue), attackModType(attackModType),
      defenceModValue(defenceModValue), defenceModType(defenceModType),
      attackModStr(attackModStr), defenceModStr(defenceModStr),
      grantsAbility(grantsAbility), grantedAbility(grantedAbility) {
    
    // Generate mod strings if not provided
    if (this->attackModStr.empty()) {
        this->attackModStr = generateModString(attackModValue, attackModType);
    }
    if (this->defenceModStr.empty()) {
        this->defenceModStr = generateModString(defenceModValue, defenceModType);
    }
}

bool Enchantment::isValidTarget(Target target) const {
    // Enchantments can only target minions
    switch (target) {
        case Target::P1B1: case Target::P1B2: case Target::P1B3: 
        case Target::P1B4: case Target::P1B5:
        case Target::P2B1: case Target::P2B2: case Target::P2B3: 
        case Target::P2B4: case Target::P2B5:
            return true;
        default:
            return false;
    }
}

void Enchantment::execute(const State& state, Player* owner, Player* other) {
    // Determine which minion to target based on state.target
    Minion* targetMinion = nullptr;
    
    switch (state.target) {
        // Player 1 (owner) board positions
        case Target::P1B1:
            if (owner->board.size() > 0) targetMinion = owner->board[0].get();
            break;
        case Target::P1B2:
            if (owner->board.size() > 1) targetMinion = owner->board[1].get();
            break;
        case Target::P1B3:
            if (owner->board.size() > 2) targetMinion = owner->board[2].get();
            break;
        case Target::P1B4:
            if (owner->board.size() > 3) targetMinion = owner->board[3].get();
            break;
        case Target::P1B5:
            if (owner->board.size() > 4) targetMinion = owner->board[4].get();
            break;
            
        // Player 2 (other) board positions  
        case Target::P2B1:
            if (other->board.size() > 0) targetMinion = other->board[0].get();
            break;
        case Target::P2B2:
            if (other->board.size() > 1) targetMinion = other->board[1].get();
            break;
        case Target::P2B3:
            if (other->board.size() > 2) targetMinion = other->board[2].get();
            break;
        case Target::P2B4:
            if (other->board.size() > 3) targetMinion = other->board[3].get();
            break;
        case Target::P2B5:
            if (other->board.size() > 4) targetMinion = other->board[4].get();
            break;
            
        default:
            // Invalid target for enchantment
            return;
    }
    
    // If we found a valid target minion, add this enchantment to it
    if (targetMinion) {
        // Create a copy of this enchantment to add to the minion
        // Note: You'll need to implement a copy constructor or clone method
        auto enchantmentCopy = std::make_unique<Enchantment>(
            this->name, this->cost,
            this->attackModValue, this->attackModType,
            this->defenceModValue, this->defenceModType,
            this->attackModStr, this->defenceModStr,
            this->grantsAbility, this->grantedAbility
        );
        
        targetMinion->addEnchantment(std::move(enchantmentCopy));
    }
}

void Enchantment::displayCard() const {
    // Display implementation will be handled elsewhere
}

std::string Enchantment::generateModString(int value, ModType type) const {
    switch (type) {
        case ModType::ADD:
            return (value >= 0 ? "+" : "") + std::to_string(value);
        case ModType::MULTIPLY:
            return "*" + std::to_string(value);
        case ModType::SET:
            return "=" + std::to_string(value);
        default:
            return "";
    }
}

// ==================== Ritual Class ====================

Ritual::Ritual(const std::string& name, int cost, int charges, int activationCost,
               std::function<void(Player*)> effect)
    : Card(name, cost), charges(charges), activationCost(activationCost), 
      ritualEffect(effect) {}

void Ritual::execute(const State& state, Player* player, Player* other) {
    
    
    charges -= activationCost;
    if (ritualEffect) {
        ritualEffect(player);
    }

}


    

void Ritual::displayCard() const {
    // Display implementation will be handled elsewhere
}
// ===== Card.cc =====
#include "Card.h"
#include "Player.h"
#include "CardInfo.h"
#include "ascii_graphics.h"
#include <typeinfo>

// --------- Base Class ---------
Card::Card(const std::string& name, int cost)
    : name(name), cost(cost), owner(nullptr), description("") {}

const std::string& Card::getName() const {
    return name;
}

// --------- Minion ---------
Minion::Minion(const std::string& name, int cost, int attack, int defence)
    : Card(name, cost), baseAttack(attack), baseDefence(defence),
      attack(attack), defence(defence), actions(0) {}

void Minion::attackPlayer(Player* target) {
    if (!target) return;
    target->setHealth(target->getHealth() - attack);
}

int Minion::getDefence() const {
    return defence;
}

int Minion::getAttack() const {
    return attack;
}



void Minion::attackMinion(Minion* target) {
    if (!target) return;
    target->defence -= attack;
    this->defence -= target->attack;
}

bool Minion::hasTrigger() const {
    return activatedAbility != nullptr;  // or whatever logic you want
}


void Minion::modifyStats(int attackMod, int defenceMod) {
    attack += attackMod;
    defence += defenceMod;
}

void Minion::addEnchantment(std::unique_ptr<Enchantment> enchantment) {
    if (!enchantment) return;

    int attackMod = enchantment->getAttackModValue();
    int defenceMod = enchantment->getDefenceModValue();

    switch (enchantment->getAttackModType()) {
        case Enchantment::ModType::ADD: attack += attackMod; break;
        case Enchantment::ModType::MULTIPLY: attack *= attackMod; break;
        case Enchantment::ModType::SET: attack = attackMod; break;
    }

    switch (enchantment->getDefenceModType()) {
        case Enchantment::ModType::ADD: defence += defenceMod; break;
        case Enchantment::ModType::MULTIPLY: defence *= defenceMod; break;
        case Enchantment::ModType::SET: defence = defenceMod; break;
    }

    enchantments.push_back(std::move(enchantment));
}

void Minion::removeAllEnchantments() {
    attack = baseAttack;
    defence = baseDefence;
    enchantments.clear();
}

std::string Minion::getType() const {
    return "Minion";
}

card_template_t Minion::render() const {
    const auto& info = CardInfo::get(name);
    if (info.actCost > 0) {
        return display_minion_activated_ability(info.name, info.cost, info.atk, info.hp, info.actCost, info.desc);
    } else if (!info.desc.empty()) {
        return display_minion_triggered_ability(info.name, info.cost, info.atk, info.hp, info.desc);
    } else {
        return display_minion_no_ability(info.name, info.cost, info.atk, info.hp);
    }
}

// --------- Spell ---------
Spell::Spell(const std::string& name, int cost,
             std::function<void(const State&, Player*, Player*)> effect,
             bool needsTarget)
    : Card(name, cost), spellEffect(effect), needsTarget(needsTarget) {}

void Spell::execute(const State& state, Player* owner, Player* other) {
    if (spellEffect) spellEffect(state, owner, other);
}

std::string Spell::getType() const {
    return "Spell";
}

card_template_t Spell::render() const {
    const auto& info = CardInfo::get(name);
    return display_spell(info.name, info.cost, info.desc);
}

// --------- Enchantment ---------
Enchantment::Enchantment(const std::string& name, int cost,
                         int attackModValue, ModType attackModType,
                         int defenceModValue, ModType defenceModType,
                         const std::string& attackModStr, const std::string& defenceModStr,
                         bool grantsAbility,
                         std::function<void(const State&, Player*)> grantedAbility)
    : Card(name, cost),
      attackModValue(attackModValue), attackModType(attackModType),
      defenceModValue(defenceModValue), defenceModType(defenceModType),
      attackModStr(attackModStr), defenceModStr(defenceModStr),
      grantsAbility(grantsAbility), grantedAbility(grantedAbility) {}

void Enchantment::execute(const State& state, Player* owner, Player* other) {
    Player* targetPlayer = nullptr;
    int index = -1;

    // Determine which player's minion to enchant based on the target
    switch (state.target) {
        case Target::P1B1: case Target::P1B2: case Target::P1B3:
        case Target::P1B4: case Target::P1B5:
            targetPlayer = owner;
            index = static_cast<int>(state.target) - static_cast<int>(Target::P1B1);
            break;
        case Target::P2B1: case Target::P2B2: case Target::P2B3:
        case Target::P2B4: case Target::P2B5:
            targetPlayer = other;
            index = static_cast<int>(state.target) - static_cast<int>(Target::P2B1);
            break;
        default:
            return; // Invalid target
    }

    Minion* targetMinion = targetPlayer->getMinion(index);
    if (targetMinion) {
        targetMinion->addEnchantment(std::make_unique<Enchantment>(*this));
    }
}


bool Enchantment::isValidTarget(Target target) const {
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

std::string Enchantment::getType() const {
    return "Enchantment";
}

card_template_t Enchantment::render() const {
    const auto& info = CardInfo::get(name);
    if (info.atk != 0 || info.hp != 0) {
        std::string a = (info.atk > 0 ? "+" : "") + std::to_string(info.atk);
        std::string h = (info.hp > 0 ? "+" : "") + std::to_string(info.hp);
        return display_enchantment_attack_defence(info.name, info.cost, info.desc, a, h);
    } else {
        return display_enchantment(info.name, info.cost, info.desc);
    }
}

std::string Enchantment::generateModString(int value, ModType type) const {
    switch (type) {
        case ModType::ADD: return (value >= 0 ? "+" : "") + std::to_string(value);
        case ModType::MULTIPLY: return "*" + std::to_string(value);
        case ModType::SET: return "=" + std::to_string(value);
        default: return "";
    }
}

// --------- Ritual ---------
Ritual::Ritual(const std::string& name, int cost, int charges, int activationCost,
               std::function<void()> effect)
    : Card(name, cost), charges(charges), activationCost(activationCost), ritualEffect(effect) {}

void Ritual::execute(const State& state, Player* player, Player* other) {
    if (canActivate()) {
        if (ritualEffect) ritualEffect();  // Optional base effect
        charges -= activationCost;
    }
}

bool Ritual::canActivate() const {
    return charges >= activationCost;
}


std::string Ritual::getType() const {
    return "Ritual";
}

card_template_t Ritual::render() const {
    const auto& info = CardInfo::get(name);
    return display_ritual(info.name, info.cost, info.actCost, info.desc, info.charges);
}

void Ritual::consumeCharges() {
    charges -= activationCost;
}

int Ritual::getCharges() const {
    return charges;
}

int Ritual::getChargeCost() const {
    return activationCost;
}

std::string Ritual::getDescription() const {
    return description;
}



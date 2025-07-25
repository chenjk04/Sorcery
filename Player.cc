#include "Player.h"
#include "CardInfo.h"
#include "ascii_graphics.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Player::Player(const std::string& name)
    : name_(name), rng(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())) {}

Player::~Player() {}

void Player::setOpponent(Player* p) {
    opponent = p;
}

Player* Player::getOpponent() const {
    return opponent;
}

int Player::getId() const {
    return this == opponent->getOpponent() ? 1 : 2;
}


void Player::loadDeckFromFile(const std::string& filename) {
    std::ifstream in(filename);
    std::string cardName;
    while (std::getline(in, cardName)) {
        if (cardName.empty()) continue;
        const auto& info = CardInfo::get(cardName);
        if (info.type == "Minion") {
            deck.emplace_back(std::make_unique<Minion>(cardName, info.cost, info.atk, info.hp));
        } else if (info.type == "Spell") {
            deck.emplace_back(std::make_unique<Spell>(cardName, info.cost, [](const State&, Player*, Player*){}, false));
        } else if (info.type == "Enchantment") {
            deck.emplace_back(std::make_unique<Enchantment>(cardName, info.cost, info.atk,
                Enchantment::ModType::ADD, info.hp, Enchantment::ModType::ADD,
                "", "", false, nullptr));
        } else if (info.type == "Ritual") {
            deck.emplace_back(std::make_unique<Ritual>(cardName, info.cost, info.charges, info.actCost, []{}));
        }
    }
    std::reverse(deck.begin(), deck.end());
}

void Player::shuffleDeck() {
    std::shuffle(deck.begin(), deck.end(), rng);
}

void Player::drawCard() {
    if (!deck.empty() && hand.size() < 5) {
        hand.push_back(std::move(deck.back()));
        deck.pop_back();
    }
}

void Player::printHand() const {
    std::vector<card_template_t> templates;
    for (const auto& c : hand) {
        const auto& info = CardInfo::get(c->getName());
        if (info.type == "Minion") {
            if (info.actCost > 0) {
                templates.push_back(display_minion_activated_ability(info.name, info.cost, info.atk, info.hp, info.actCost, info.desc));
            } else if (!info.desc.empty()) {
                templates.push_back(display_minion_triggered_ability(info.name, info.cost, info.atk, info.hp, info.desc));
            } else {
                templates.push_back(display_minion_no_ability(info.name, info.cost, info.atk, info.hp));
            }
        } else if (info.type == "Spell") {
            templates.push_back(display_spell(info.name, info.cost, info.desc));
        } else if (info.type == "Ritual") {
            templates.push_back(display_ritual(info.name, info.cost, info.actCost, info.desc, info.charges));
        } else {
            if (info.atk != 0 || info.hp != 0) {
                std::string a = (info.atk > 0 ? "+" : "") + std::to_string(info.atk);
                std::string h = (info.hp > 0 ? "+" : "") + std::to_string(info.hp);
                templates.push_back(display_enchantment_attack_defence(info.name, info.cost, info.desc, a, h));
            } else {
                templates.push_back(display_enchantment(info.name, info.cost, info.desc));
            }
        }
    }
    for (size_t row = 0; row < templates[0].size(); ++row) {
        for (const auto& card : templates) {
            std::cout << card[row];
        }
        std::cout << '\n';
    }
}

void Player::moveMinionToGraveyard(int index) {
    if (index >= 0 && index < static_cast<int>(board.size())) {
        graveyard.push_back(std::move(board[index]));
        board.erase(board.begin() + index);
    }
}

void Player::destroyMinion(Target target) {
    int index = -1;
    int playerId = -1;

    if (target >= Target::P1B1 && target <= Target::P1B5) {
        index = static_cast<int>(target) - static_cast<int>(Target::P1B1);
        playerId = 1;
    } else if (target >= Target::P2B1 && target <= Target::P2B5) {
        index = static_cast<int>(target) - static_cast<int>(Target::P2B1);
        playerId = 2;
    }

    if ((playerId == 1 && this->getId() == 1) || (playerId == 2 && this->getId() == 2)) {
        moveMinionToGraveyard(index);
    }
}


void Player::playCard(int handIndex, const State& state) {
    if (handIndex < 0 || handIndex >= static_cast<int>(hand.size())) return;

    auto card = std::move(hand[handIndex]);
    hand.erase(hand.begin() + handIndex);

    std::string type = card->getType();

    if (magic_ < card->getCost()) {
    std::cerr << "Not enough magic to play " << card->getName() << "!\n";
    return;
}
magic_ -= card->getCost();


    if (type == "Ritual") {
        ritual = std::move(card);
    }
    else if (type == "Minion") {
        board.emplace_back(std::move(card));
        allyEnter();
        opponent->anyEnter();
    }
    else if (type == "Spell") {
        Spell* s = dynamic_cast<Spell*>(card.get());
        if (s && s->requiresTarget()) {
            if (state.target != Target::NONE) {
                s->execute(state, this, opponent);
            } else {
                std::cerr << "Target required but not provided.\n";
            }
        } else if (s) {
            s->execute(state, this, opponent);
        }
    }
    else if (type == "Enchantment") {
        if (state.target != Target::NONE) {
            card->execute(state, this, opponent);
        } else {
            std::cerr << "Target required but not provided.\n";
        }
    }
}


int source_to_index(State s) {
    switch (s.source) {
        case Source::B1: return 0;
        case Source::B2: return 1;
        case Source::B3: return 2;
        case Source::B4: return 3;
        case Source::B5: return 4;
        default:
            throw std::invalid_argument("Invalid source for board index");
    }
}

int hand_to_index(State s) {
    switch (s.source) {
        case Source::H1: return 0;
        case Source::H2: return 1;
        case Source::H3: return 2;
        case Source::H4: return 3;
        case Source::H5: return 4;
        default:
            throw std::invalid_argument("Invalid source for hand index");
    }
}

int oppo_to_index(State s) {
    switch (s.target) {
        case Target::P2B1: return 0;
        case Target::P2B2: return 1;
        case Target::P2B3: return 2;
        case Target::P2B4: return 3;
        case Target::P2B5: return 4;
        default:
            throw std::invalid_argument("Invalid target for opponent board index");
    }
}


void Player::attack(const State& state) {
    int attackerIndex = static_cast<int>(state.source) - static_cast<int>(Source::B1);
    Minion* attacker = getMinion(attackerIndex);
    if (!attacker) return;
    

    if (state.target == Target::player1 || state.target == Target::player2) {
        opponent->setHealth(opponent->getHealth() - attacker->getDefence());
    } else {
        int targetIndex = -1;
        Player* defender = this;

        if (state.target >= Target::P2B1 && state.target <= Target::P2B5) {
            targetIndex = static_cast<int>(state.target) - static_cast<int>(Target::P2B1);
            defender = opponent;
        } else if (state.target >= Target::P1B1 && state.target <= Target::P1B5) {
            targetIndex = static_cast<int>(state.target) - static_cast<int>(Target::P1B1);
        }

        Minion* targetMinion = defender->getMinion(targetIndex);
        if (!targetMinion) return;

        attacker->attackMinion(targetMinion);

        if (attacker->getDefence() <= 0) {moveMinionToGraveyard(attackerIndex); allyDie(); anyDie(); opponent->anyDie();}
        if (targetMinion->getDefence() <= 0) {defender->moveMinionToGraveyard(targetIndex); anyDie(); opponent->anyDie(); opponent->allyDie();}
    }
}


void Player::use(State state) {
    int attackerIndex = static_cast<int>(state.source) - static_cast<int>(Source::B1);
    Minion* attacker = getMinion(attackerIndex);
    attacker->useAbility(opponent);
}

void Player::notify(State state) {
    if (state.action == Action::attack) {
        attack(state);
    } else if (state.action == Action::play) {
        playCard(hand_to_index(state), state);
    } else {
        use(state);
    }
}

void Player::startOfTurn() {
    magic_ = 3;
    drawCard();
    if (ritual) {
    if (ritual->canActivate()) {
        if (ritual->startOfTurnTrigger != nullptr) {
            ritual->startOfTurnTrigger(this, opponent);
            ritual->consumeCharges();
        }
    }
}
    for (auto& card : board) {
        card->startOfTurnTrigger(this, opponent);
    }
}

void Player::endOfTurn() {
    if (ritual) {
    if (ritual->canActivate()) {
        if (ritual->endOfTurnTrigger != nullptr) {
            ritual->endOfTurnTrigger(this, opponent);
            ritual->consumeCharges();
        }
    }
}
    for (auto& card : board) {
        card->endOfTurnTrigger(this, opponent);
    }
}

void Player::allyEnter() {
    if (ritual) {
    if (ritual->canActivate()) {
        if (ritual->myMinionEnter != nullptr) {
            ritual->myMinionEnter(this, opponent);
            ritual->consumeCharges();
        }
    }
}
    for (auto& card : board) {
        card->myMinionEnter(this, opponent);
    }
}
void Player::allyDie() {
    if (ritual) {
    if (ritual->canActivate()) {
        if (ritual->myMinionDie != nullptr) {
            ritual->myMinionDie(this, opponent);
            ritual->consumeCharges();
        }
    }
}
    for (auto& card : board) {
        card->myMinionDie(this, opponent);
    }
}

void Player::anyEnter() {
    if (ritual) {
    if (ritual->canActivate()) {
        if (ritual->anyMinionEnter != nullptr) {
            ritual->anyMinionEnter(this, opponent);
            ritual->consumeCharges();
        }
    }
}
    for (auto& card : board) {
        card->anyMinionEnter(this, opponent);
    }
}

void Player::anyDie() {
    if (ritual) {
    if (ritual->canActivate()) {
        if (ritual->anyMinionDie != nullptr) {
            ritual->anyMinionDie(this, opponent);
            ritual->consumeCharges();
        }
    }
}
    for (auto& card : board) {
        card->anyMinionDie(this, opponent);
    }
}




void Player::setHealth(int h) { health_ = h; }
int Player::getHealth() const { return health_; }
int Player::getMagic() const { return magic_; }
const std::string& Player::getName() const { return name_; }


Card* Player::getRitual() const {
    return ritual ? ritual.get() : nullptr;
}

Card* Player::topGraveyard() const {
    return graveyard.empty() ? nullptr : graveyard.back().get();
}

Minion* Player::getMinion(int index) const {
    if (index < 0 || index >= static_cast<int>(board.size())) return nullptr;
    return dynamic_cast<Minion*>(board[index].get());
}

void Player::inspect(int i) const {
    if (i < 0 || i >= static_cast<int>(board.size())) {
        std::cout << "Invalid minion index.\n";
        return;
    }

    Minion* minion = dynamic_cast<Minion*>(board[i].get());
    if (!minion) {
        std::cout << "Card at this index is not a minion.\n";
        return;
    }

    const auto& baseTemplate = minion->render();
    for (const auto& line : baseTemplate) {
        std::cout << line << '\n';
    }

    if (!minion->getEnchantments().empty()) return;

    const auto& enchantments = minion->getEnchantments();
    std::vector<card_template_t> stack;
    for (const auto& e : enchantments) {
        stack.push_back(e->render());
    }

    // Print in groups of 5 per line
    for (size_t i = 0; i < stack.size(); i += 5) {
        size_t groupSize = std::min(static_cast<size_t>(5), stack.size() - i);
        for (size_t line = 0; line < stack[0].size(); ++line) {
            for (size_t j = 0; j < groupSize; ++j) {
                std::cout << stack[i + j][line];
            }
            std::cout << '\n';
        }
    }
}

void Player::activateTrigger(const State& state) {
    if (ritual) {
        Ritual* r = dynamic_cast<Ritual*>(ritual.get());
        if (!r || !r->canActivate()) return;

        const std::string name = r->getName();

        if (name == "Standstill" && state.action == Action::play) {
            destroyMinion(state.target);
            r->consumeCharges();
        } else if (name == "Aura of Power" && state.action == Action::play &&
                   ((getId() == 1 && state.target >= Target::P1B1 && state.target <= Target::P1B5) ||
                    (getId() == 2 && state.target >= Target::P2B1 && state.target <= Target::P2B5))) {

            int index = static_cast<int>(state.target) % 5;
            Minion* m = getMinion(index);
            if (m) {
                m->modifyStats(1, 1); // +1/+1
                r->consumeCharges();
            }
        }
    }

    for (auto& c : board) {
        Minion* m = dynamic_cast<Minion*>(c.get());
        if (!m) continue;

        const std::string name = m->getName();

        if (name == "Bone Golem" && state.action == Action::play &&
            (state.target >= Target::P1B1 && state.target <= Target::P2B5)) {
            m->modifyStats(1, 1); // +1/+1 when any minion leaves play
        } else if (name == "Fire Elemental" && state.action == Action::play &&
                   ((getId() == 1 && state.target >= Target::P2B1 && state.target <= Target::P2B5) ||
                    (getId() == 2 && state.target >= Target::P1B1 && state.target <= Target::P1B5))) {
            int index = static_cast<int>(state.target) % 5;
            Player* enemy = getId() == 1 ? opponent : this;
            Minion* target = enemy->getMinion(index);
            if (target) target->modifyStats(0, -1); // Deal 1 damage
        }
    }
}




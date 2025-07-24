// ===== Component.h =====
#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <memory>
#include <iostream>
#include "Subject.h"
#include "Observer.h"
#include "State.h"
#include "Card.h"

class Component : public Subject, public Observer {
protected:
    std::vector<std::unique_ptr<Card>> cards_;
public:
    Component() = default;
    virtual ~Component() = default;

    // Observer callback: default does nothing
    void notify(State state) override {
        // Optional: dispatch component-specific logic
    }

    // Zone interface
    virtual void addCard(std::unique_ptr<Card> card) = 0;
    virtual std::unique_ptr<Card> removeCard(int index) = 0;
    virtual void show() const = 0;
};

#endif // COMPONENT_H


// ===== DeckComponent.h =====
#ifndef DECKCOMPONENT_H
#define DECKCOMPONENT_H

#include "Component.h"
#include <string>
#include <algorithm>
#include <random>
#include <fstream>

class DeckComponent : public Component {
    std::mt19937 rng_{std::random_device{}()};
public:
    DeckComponent(const std::string& filename) {
        std::ifstream in(filename);
        std::string name, type;
        int cost;
        while (in >> name >> cost >> type) {
            cards_.push_back(std::make_unique<Card>(Card{name, cost, type}));
        }
    }

    void shuffle() {
        std::shuffle(cards_.begin(), cards_.end(), rng_);
    }

    // draw top card
    std::unique_ptr<Card> draw() {
        if (cards_.empty()) return nullptr;
        auto card = std::move(cards_.back());
        cards_.pop_back();
        notify(State::DrawCard);
        return card;
    }

    bool empty() const { return cards_.empty(); }

    // Component interface
    void addCard(std::unique_ptr<Card> card) override {
        cards_.push_back(std::move(card));
    }

    std::unique_ptr<Card> removeCard(int index) override {
        if (index < 0 || index >= (int)cards_.size()) return nullptr;
        auto card = std::move(cards_[index]);
        cards_.erase(cards_.begin() + index);
        return card;
    }

    void show() const override {
        std::cout << "Deck (" << cards_.size() << " cards)\n";
    }
};

#endif // DECKCOMPONENT_H


// ===== HandComponent.h =====
#ifndef HANDCOMPONENT_H
#define HANDCOMPONENT_H

#include "Component.h"
#include "ascii_graphics.h"

class HandComponent : public Component {
public:
    void addCard(std::unique_ptr<Card> card) override {
        cards_.push_back(std::move(card));
    }

    std::unique_ptr<Card> removeCard(int index) override {
        if (index < 0 || index >= (int)cards_.size()) return nullptr;
        auto c = std::move(cards_[index]);
        cards_.erase(cards_.begin() + index);
        return c;
    }

    void show() const override {
        for (const auto& c : cards_) {
            const card_template_t& tpl =
                c->type == "Minion" ? CARD_TEMPLATE_MINION_NO_ABILITY : CARD_TEMPLATE_EMPTY;
            for (const auto& line : tpl) std::cout << line << "\n";
            std::cout << "\n";
        }
    }
};

#endif // HANDCOMPONENT_H


// ===== BoardComponent.h =====
#ifndef BOARDCOMPONENT_H
#define BOARDCOMPONENT_H

#include "Component.h"

class BoardComponent : public Component {
public:
    void addCard(std::unique_ptr<Card> card) override {
        cards_.push_back(std::move(card));
    }

    std::unique_ptr<Card> removeCard(int index) override {
        if (index < 0 || index >= (int)cards_.size()) return nullptr;
        auto c = std::move(cards_[index]);
        cards_.erase(cards_.begin() + index);
        return c;
    }

    void show() const override {
        // Render this board in a 5-slot external box using ascii_graphics
        // Build slots: up to 5 cards
        std::vector<const card_template_t*> slots;
        for (int i = 0; i < 5; ++i) {
            if (i < (int)cards_.size()) {
                const Card& c = *cards_[i];
                if (c.type == "Minion") {
                    slots.push_back(&CARD_TEMPLATE_MINION_NO_ABILITY);
                } else {
                    slots.push_back(&CARD_TEMPLATE_EMPTY);
                }
            } else {
                slots.push_back(&CARD_TEMPLATE_EMPTY);
            }
        }
        // compute dimensions
        int cardW = CARD_TEMPLATE_EMPTY[0].size();
        int totalW = 5 * cardW;
        // top border
        std::cout
            << EXTERNAL_BORDER_CHAR_TOP_LEFT
            << std::string(totalW, EXTERNAL_BORDER_CHAR_UP_DOWN[0])
            << EXTERNAL_BORDER_CHAR_TOP_RIGHT << "
";
        // content rows
        size_t rows = slots.front()->size();
        for (size_t r = 0; r < rows; ++r) {
            std::cout << EXTERNAL_BORDER_CHAR_LEFT_RIGHT;
            for (auto tpl : slots) {
                std::cout << (*tpl)[r];
            }
            std::cout << EXTERNAL_BORDER_CHAR_LEFT_RIGHT << "
";
        }
        // bottom border
        std::cout
            << EXTERNAL_BORDER_CHAR_BOTTOM_LEFT
            << std::string(totalW, EXTERNAL_BORDER_CHAR_UP_DOWN[0])
            << EXTERNAL_BORDER_CHAR_BOTTOM_RIGHT << "
";
    }
};

#endif // BOARDCOMPONENT_H


// ===== GraveyardComponent.h =====
#ifndef GRAVEYARDCOMPONENT_H
#define GRAVEYARDCOMPONENT_H

#include "Component.h"
#include <iostream>

class GraveyardComponent : public Component {
public:
    void addCard(std::unique_ptr<Card> card) override {
        cards_.push_back(std::move(card));
    }

    std::unique_ptr<Card> removeCard(int /*index*/) override {
        if (cards_.empty()) return nullptr;
        auto c = std::move(cards_.back());
        cards_.pop_back();
        return c;
    }

    void show() const override {
        std::cout << "Graveyard (" << cards_.size() << " cards)\n";
    }
};

#endif


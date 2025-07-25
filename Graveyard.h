#ifndef GRAVEYARD_H
#define GRAVEYARD_H

#include <vector>
#include <memory>
#include "Card.h"

class Graveyard {
public:
    void add(std::unique_ptr<Minion> m) {
        minions_.push_back(std::move(m));
    }

    Minion* top() const {
        return minions_.empty() ? nullptr : minions_.back().get();
    }

    std::unique_ptr<Minion> removeTop() {
        if (minions_.empty()) return nullptr;
        auto m = std::move(minions_.back());
        minions_.pop_back();
        return m;
    }

    bool empty() const {
        return minions_.empty();
    }

private:
    std::vector<std::unique_ptr<Minion>> minions_;
};

#endif

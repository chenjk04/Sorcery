#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <memory>
#include "Subject.h"
#include "Observer.h"
#include "State.h"
#include "Card.h"
class Component : public Subject, Observer {
    std::vector<std::unique_ptr<Card>> cards;

public:
    void notify(State state) override;
};
#endif



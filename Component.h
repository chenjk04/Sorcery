#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <memory>

#include "Observer.h"
#include "State.h"
class Component : public Observer {
    std::vector<std::unique_ptr> cards;

public:
    notify(State state) override;
}
#endif
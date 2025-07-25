#ifndef OBSERVER_H
#define OBSERVER_H

#include "State.h"

class Observer {
public:
    virtual void notify(State state) = 0;
};

#endif

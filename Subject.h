#ifndef SUBJECT_H
#define SUBJECT_H

#include "State.h"

class Subject {
public:
    virtual void notifyObserver(State state) = 0;
};

#endif

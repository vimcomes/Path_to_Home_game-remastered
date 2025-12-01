#pragma once
#include "Meteor.h"
class Fuel :
    public Meteor
{
public:
    Fuel(String name, int x, int y);
    ~Fuel();

    virtual void restart();

private:
    using Meteor::animation;
};


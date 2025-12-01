#pragma once
#include "Meteor.h"
#include <SFML/Graphics.hpp>

// Generic pickup bonus type
enum class BonusType { Fuel, Shield };

class Fuel :
    public Meteor
{
public:
    Fuel(const sf::String& name, int x, int y); // load texture and size for fuel canister
    ~Fuel();

    virtual void restart(); // respawn fuel on the right side
    int getAmount() const { return amount; }
    BonusType getType() const { return type; }

private:
    using Meteor::animation;
    void updateScaleFromAmount();
    int amount = 0;
    float baseScale = 1.f;
    BonusType type = BonusType::Fuel;
};

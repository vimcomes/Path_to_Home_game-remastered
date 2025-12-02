#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameState.h"

// ParticleSystem owns short-lived smoke and flash particles.
class ParticleSystem
{
public:
    void emitSmoke(const sf::Vector2f& position, float size, float ttl);
    void emitFlash(const sf::Vector2f& position, float ttl);

    void update(float dt, const game::Config& cfg);
    void drawSmoke(sf::RenderTarget& target) const;
    void drawFlashes(sf::RenderTarget& target) const;

    void clear();

private:
    struct SmokeParticle
    {
        sf::Vector2f pos{};
        float lifetime = 0.f;
        float ttl = 0.f;
        float size = 0.f;
    };

    struct PickupFlash
    {
        sf::Vector2f pos{};
        float lifetime = 0.f;
        float ttl = 0.f;
    };

    std::vector<SmokeParticle> smoke_;
    std::vector<PickupFlash> flashes_;
};

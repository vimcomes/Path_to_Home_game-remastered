#include "ParticleSystem.h"
#include <algorithm>
#include <cmath>

void ParticleSystem::emitSmoke(const sf::Vector2f& position, float size, float ttl) {
    smoke_.push_back(SmokeParticle{position, ttl, ttl, size});
}

void ParticleSystem::emitFlash(const sf::Vector2f& position, float ttl) {
    flashes_.push_back(PickupFlash{position, ttl, ttl});
}

void ParticleSystem::update(float dt, const game::Config& cfg) {
    for (auto it = smoke_.begin(); it != smoke_.end();) {
        it->lifetime -= dt;
        it->pos.x -= cfg.backgroundSpeed * dt * 0.5f;
        it->pos.y -= 10.f * dt;
        if (it->lifetime <= 0.f) {
            it = smoke_.erase(it);
        } else {
            ++it;
        }
    }

    std::erase_if(flashes_, [dt](PickupFlash& f) {
        f.lifetime -= dt;
        return f.lifetime <= 0.f;
    });
}

void ParticleSystem::drawSmoke(sf::RenderTarget& target) const {
    for (const auto& s : smoke_) {
        sf::CircleShape puff(s.size);
        float alpha = std::max(0.f, s.lifetime) / s.ttl;
        float faded = std::pow(alpha, 0.8f);
        puff.setFillColor(sf::Color(
            230,
            230,
            230,
            static_cast<sf::Uint8>(170 * faded)));
        puff.setPosition(s.pos);
        target.draw(puff);
    }
}

void ParticleSystem::drawFlashes(sf::RenderTarget& target) const {
    for (const auto& f : flashes_) {
        float k = f.lifetime / f.ttl;
        float radius = 10.f + 25.f * (1.f - k);
        sf::CircleShape flash(radius);
        flash.setOrigin(radius, radius);
        flash.setPosition(f.pos);
        flash.setFillColor(sf::Color(255, 200, 80, static_cast<sf::Uint8>(190 * k)));
        target.draw(flash);
        sf::CircleShape flashCore(radius * 0.5f);
        flashCore.setOrigin(radius * 0.5f, radius * 0.5f);
        flashCore.setPosition(f.pos);
        flashCore.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(200 * k)));
        target.draw(flashCore);
    }
}

void ParticleSystem::clear() {
    smoke_.clear();
    flashes_.clear();
}

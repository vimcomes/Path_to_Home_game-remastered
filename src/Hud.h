#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "GameState.h"

class Hud
{
public:
    Hud() = default;

    void initialize(const game::Resources& res, const game::Config& cfg,
                    const game::GameState& state);
    void update(const game::GameState& state, const game::Config& cfg);
    void draw(sf::RenderTarget& target, int lives) const;
    void drawFuelGain(sf::RenderTarget& target, int pusk) const;

    void moveProgress(float dx);
    void setProgressToEnd();
    void resetProgress();

    [[nodiscard]] float progressX() const noexcept;
    [[nodiscard]] float progressEnd() const noexcept;
    [[nodiscard]] float progressStart() const noexcept;

    void setFuelGainShieldText(const sf::Vector2f& pos);
    void setFuelGainAmountText(const std::string& amountText);
    void animateFuelGain(int& pusk);

private:
    void initLifeIcons(const game::Resources& res, int lives);

    sf::RectangleShape gameInfoPanel;
    sf::RectangleShape progressBg;
    sf::RectangleShape progressFill;
    sf::RectangleShape progressShine;
    sf::RectangleShape fuelBg;
    sf::RectangleShape fuelFill;
    sf::RectangleShape fuelShine;
    sf::CircleShape progressMarker;
    sf::CircleShape progressMarkerGlow;

    sf::Text progressLabel;
    sf::Text fuelLabel;
    sf::Text fuelMax;
    sf::Text fuelGainText;

    std::vector<sf::Sprite> lifeIcons;

    float progressStartValue = 0.f;
    float progressEndValue = 0.f;
    float progressMarkerY = 0.f;
};

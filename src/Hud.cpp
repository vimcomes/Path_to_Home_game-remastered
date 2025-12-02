#include "Hud.h"
#include <algorithm>

void Hud::initialize(const game::Resources& res, const game::Config&, const game::GameState& state) {
    gameInfoPanel = sf::RectangleShape(sf::Vector2f(1280.f, 113.f));
    gameInfoPanel.setTexture(&res.panel);
    gameInfoPanel.setPosition(sf::Vector2f(0.f, 0.f));

    progressBg = sf::RectangleShape(sf::Vector2f(300.f, 14.f));
    progressBg.setFillColor(sf::Color(40, 40, 40, 200));
    progressBg.setOutlineColor(sf::Color(10, 10, 10, 200));
    progressBg.setOutlineThickness(1.5f);
    progressBg.setPosition(520.f, 60.f);
    progressFill = sf::RectangleShape(sf::Vector2f(0.f, 14.f));
    progressFill.setFillColor(sf::Color(100, 210, 255));
    progressFill.setPosition(progressBg.getPosition());
    progressShine = sf::RectangleShape(sf::Vector2f(0.f, 6.f));
    progressShine.setFillColor(sf::Color(180, 230, 255, 140));
    progressShine.setPosition(progressBg.getPosition().x, progressBg.getPosition().y + 4.f);

    fuelBg = sf::RectangleShape(sf::Vector2f(220.f, 14.f));
    fuelBg.setFillColor(sf::Color(40, 40, 40, 200));
    fuelBg.setOutlineColor(sf::Color(10, 10, 10, 200));
    fuelBg.setOutlineThickness(1.5f);
    fuelBg.setPosition(1040.f, 60.f);
    fuelFill = sf::RectangleShape(sf::Vector2f(0.f, 14.f));
    fuelFill.setFillColor(sf::Color::Green);
    fuelFill.setPosition(fuelBg.getPosition());
    fuelShine = sf::RectangleShape(sf::Vector2f(0.f, 6.f));
    fuelShine.setFillColor(sf::Color(200, 255, 200, 140));
    fuelShine.setPosition(fuelBg.getPosition().x, fuelBg.getPosition().y + 4.f);

    progressMarker = sf::CircleShape(5.f);
    progressMarker.setFillColor(sf::Color(255, 0, 0));
    progressMarker.setOutlineThickness(2.f);
    progressMarker.setOutlineColor(sf::Color(255, 155, 0));
    progressMarkerGlow = sf::CircleShape(11.f);
    progressMarkerGlow.setFillColor(sf::Color(255, 180, 50, 120));

    progressStartValue = progressBg.getPosition().x;
    progressEndValue = progressBg.getPosition().x + progressBg.getSize().x;
    progressMarkerY = progressBg.getPosition().y + progressBg.getSize().y / 2.f;
    progressMarker.setPosition(progressStartValue, progressMarkerY);

    progressLabel.setFont(res.font);
    progressLabel.setCharacterSize(18);
    progressLabel.setFillColor(sf::Color(230, 230, 230));
    progressLabel.setOutlineColor(sf::Color(0, 0, 0, 130));
    progressLabel.setOutlineThickness(1.f);
    progressLabel.setString(L"Прогресс");
    progressLabel.setPosition(progressBg.getPosition().x - 120.f, progressBg.getPosition().y - 4.f);

    fuelLabel.setFont(res.font);
    fuelLabel.setCharacterSize(18);
    fuelLabel.setFillColor(sf::Color(230, 230, 230));
    fuelLabel.setOutlineColor(sf::Color(0, 0, 0, 130));
    fuelLabel.setOutlineThickness(1.f);
    fuelLabel.setString(L"Топливо");
    fuelLabel.setPosition(fuelBg.getPosition().x - 100.f, fuelBg.getPosition().y - 4.f);

    fuelMax.setFont(res.font);
    fuelMax.setCharacterSize(14);
    fuelMax.setFillColor(sf::Color(220, 220, 220));
    fuelMax.setOutlineColor(sf::Color(0, 0, 0, 130));
    fuelMax.setOutlineThickness(1.f);
    fuelMax.setString(L"MAX");
    fuelMax.setPosition(fuelBg.getPosition().x + fuelBg.getSize().x + 6.f,
                        fuelBg.getPosition().y - 2.f);

    fuelGainText.setFont(res.font);
    fuelGainText.setFillColor(sf::Color::Green);
    fuelGainText.setCharacterSize(25);
    fuelGainText.setOutlineColor(sf::Color(0, 0, 0, 160));
    fuelGainText.setOutlineThickness(1.f);

    initLifeIcons(res, state.lives);
}

void Hud::update(const game::GameState& state, const game::Config& cfg) {
    float fuelPercent = std::clamp(state.fuel / cfg.maxFuel, 0.f, 1.f);
    float fuelWidth = fuelBg.getSize().x * fuelPercent;
    fuelFill.setSize(sf::Vector2f(fuelWidth, fuelBg.getSize().y));
    fuelShine.setSize(sf::Vector2f(fuelWidth, fuelShine.getSize().y));
    if (fuelPercent >= 0.5f)
        fuelFill.setFillColor(sf::Color::Green);
    else if (fuelPercent >= 0.25f)
        fuelFill.setFillColor(sf::Color::Yellow);
    else
        fuelFill.setFillColor(sf::Color::Red);

    float progressPercent = std::clamp(
        (progressMarker.getPosition().x - progressStartValue) / (progressEndValue - progressStartValue),
        0.f, 1.f);
    float progressWidth = progressBg.getSize().x * progressPercent;
    progressFill.setSize(sf::Vector2f(progressWidth, progressBg.getSize().y));
    progressShine.setSize(sf::Vector2f(progressWidth, progressShine.getSize().y));
}

void Hud::draw(sf::RenderTarget& target, int lives) const {
    target.draw(gameInfoPanel);
    target.draw(progressBg);
    target.draw(progressFill);
    target.draw(progressShine);
    target.draw(progressLabel);
    target.draw(fuelBg);
    target.draw(fuelFill);
    target.draw(fuelShine);
    target.draw(fuelLabel);
    target.draw(fuelMax);
    for (int i = 0; i < lives && i < static_cast<int>(lifeIcons.size()); ++i)
        target.draw(lifeIcons[i]);

    sf::CircleShape glow = progressMarkerGlow;
    glow.setPosition(
        progressMarker.getPosition().x - progressMarkerGlow.getRadius() + progressMarker.getRadius(),
        progressMarker.getPosition().y - progressMarkerGlow.getRadius() + progressMarker.getRadius());
    target.draw(glow);
    target.draw(progressMarker);
}

void Hud::drawFuelGain(sf::RenderTarget& target, int pusk) const {
    if (pusk > 0)
        target.draw(fuelGainText);
}

void Hud::moveProgress(float dx) {
    float newX = std::min(progressMarker.getPosition().x + dx, progressEndValue);
    progressMarker.setPosition(newX, progressMarkerY);
}

void Hud::setProgressToEnd() {
    progressMarker.setPosition(progressEndValue, progressMarkerY);
}

void Hud::resetProgress() {
    progressMarker.setPosition(progressStartValue, progressMarkerY);
}

float Hud::progressX() const noexcept {
    return progressMarker.getPosition().x;
}

float Hud::progressEnd() const noexcept {
    return progressEndValue;
}

float Hud::progressStart() const noexcept {
    return progressStartValue;
}

void Hud::setFuelGainShieldText(const sf::Vector2f& pos) {
    fuelGainText.setString(L"SHIELD");
    fuelGainText.setFillColor(sf::Color::Red);
    fuelGainText.setPosition(pos);
}

void Hud::setFuelGainAmountText(const std::string& amountText) {
    fuelGainText.setString(amountText);
    fuelGainText.setFillColor(sf::Color::Green);
    fuelGainText.setPosition(fuelBg.getPosition().x + fuelBg.getSize().x - 30.f,
                             fuelBg.getPosition().y + 22.f);
}

void Hud::animateFuelGain(int& pusk) {
    if (pusk > 0) {
        --pusk;
        fuelGainText.move(0.f, -1.f);
    }
}

void Hud::initLifeIcons(const game::Resources& res, int lives) {
    lifeIcons.clear();
    sf::Sprite lifeSprite(res.life);
    lifeSprite.setTextureRect(sf::IntRect(0, 700, 90, 90));
    lifeSprite.setScale(0.25f, 0.25f);
    for (int i = 0; i < lives; ++i) {
        sf::Sprite icon = lifeSprite;
        icon.setPosition(30.f + i * 50.f, 75.f);
        lifeIcons.push_back(icon);
    }
}

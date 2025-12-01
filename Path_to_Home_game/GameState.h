#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

namespace game
{

struct FrameAnim
{
    int Frame = 0;
    int Step = 100;
    int Frame1 = 0;
    int Step1 = 100;
};

struct SmokeParticle
{
    sf::Vector2f pos;
    float lifetime = 0.f;
    float ttl = 0.f;
    float size = 0.f;
};

struct PickupFlash
{
    sf::Vector2f pos;
    float lifetime = 0.f;
    float ttl = 0.f;
};

struct GameState
{
    bool gameOver = false;
    bool gamePause = false;
    float deltaSeconds = 0.f;
    float smokeAccumulator = 0.f;
    float shakeTimer = 0.f;
    float flashTimer = 0.f;
    float shieldTimer = 0.f;
    bool shieldActive = false;
    float shieldPulse = 0.f;
    bool victoryPlayed = false;
    int pusk = 0;
    double fuelGain = 0;
    float fuel = 200.f;
    int lives = 5;
    sf::Vector2f moveRec{};
    int traffic = 0;
    FrameAnim playerAnim{700, 100, 0, 100};
    FrameAnim destructAnim{5, 100, 15, 100};
    std::vector<SmokeParticle> smoke;
    std::vector<PickupFlash> flashes;
};

struct Config
{
    float playerSpeed = 260.f;         // px/sec
    float backgroundSpeed = 72.f;      // px/sec
    float backgroundSpeedFar = 180.f;  // px/sec
    float progressSpeed = 1.25f;       // px/sec
    float fuelDrainPerSecond = 12.5f;  // units/sec when thrusting
    float maxFuel = 220.f;
    float shieldDuration = 6.f;
};

struct Resources
{
    sf::Texture panel;
    sf::Texture background;
    sf::Texture life;
    sf::Texture earth;
    sf::Texture player;
    sf::Texture explosion;
    sf::Font font;
    sf::SoundBuffer sbFail, sbBonusM, sbBonusB, sbVictory;
};

struct HudSprites
{
    std::vector<sf::Sprite> lifeIcons;
};

constexpr int Nmeteor = 15;

} // namespace game


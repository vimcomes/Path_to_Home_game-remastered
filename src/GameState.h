#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Vector2.hpp>
#include <random>

namespace game
{

struct FrameAnim
{
    int Frame = 0;
    int Step = 100;
    int Frame1 = 0;
    int Step1 = 100;
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
    bool gameOverFinal = false;
    int lives = 5;
    sf::Vector2f moveRec{};
    int traffic = 0;
    FrameAnim playerAnim{700, 100, 0, 100};
    FrameAnim destructAnim{5, 100, 15, 100};
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

constexpr int Nmeteor = 15;

namespace constants {

constexpr float WindowWidth = 1280.f;
constexpr float WindowHeight = 720.f;
constexpr float PlayerStartX = 80.f;
constexpr float PlayerStartY = 380.f;
constexpr float PlayerClampLeft = 50.f;
constexpr float PlayerClampRight = 1200.f;
constexpr float PlayerClampTop = 110.f;
constexpr float PlayerClampBottom = 660.f;
constexpr float MeteorSpeed = 160.f;
constexpr float SmokeEmitInterval = 0.04f;

} // namespace constants

inline std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

inline int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng());
}

inline float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng());
}

} // namespace game

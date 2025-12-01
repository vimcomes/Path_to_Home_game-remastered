#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include "Fuel.h"
#include "GameState.h"
#include "Meteor.h"

class Game
{
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();
    void updateGameplay(float dt);
    void updateParticles(float dt);
    void updateBackground(float dt);
    void checkCollisions();
    void updateHUD();
    void renderGameplay();
    void renderVictory();
    void renderPause();
    void renderSmoke();
    void renderFlashes();
    void renderPlayer();
    void drawHUD();
    void showFinalGameOver();
    void initResources();
    void initWindowIcon();
    void initAudio();
    void initHUD();
    void initBackground();
    void initPlayer();
    void initTexts();
    void initLifeIcons();
    void setVictoryIfReached();
    void handleGameOverAnimation();
    void updateFuelGainText();

    sf::RenderWindow window;
    sf::View baseView{};

    game::Resources res{};
    game::Config cfg{};
    game::GameState state{};
    game::HudSprites hud{};

    bool musicEnabled = true;
    sf::Music gameMusic;
    sf::Sound failSound;
    sf::Sound bonusSmallSound;
    sf::Sound bonusBigSound;
    sf::Sound victorySound;

    sf::RectangleShape gameInfoPanel;
    sf::RectangleShape progressBg;
    sf::RectangleShape progressFill;
    sf::RectangleShape progressShine;
    sf::RectangleShape fuelBg;
    sf::RectangleShape fuelFill;
    sf::RectangleShape fuelShine;
    sf::RectangleShape pauseOverlay;
    sf::RectangleShape gamingBackground;
    sf::RectangleShape gamingBackground2;
    sf::RectangleShape bgFar1;
    sf::RectangleShape bgFar2;
    sf::RectangleShape vignette;
    sf::RectangleShape earth;
    sf::CircleShape progressMarker;
    sf::CircleShape progressMarkerGlow;

    sf::Text progressLabel;
    sf::Text fuelLabel;
    sf::Text fuelMax;
    sf::Text fuelGainText;
    sf::Text textPause;
    sf::Text winTitle;
    sf::Text winSub;

    sf::Sprite player;
    sf::Sprite destruction;

    std::array<Meteor, game::Nmeteor> meteors{};
    Fuel canister;

    sf::Clock deltaClock;
    sf::Clock animPlayClock;
    sf::Clock animMeteorClock;
    sf::Clock animTextClock;

    float progressStart = 0.f;
    float progressEnd = 0.f;
    float progressMarkerY = 0.f;
    bool thrustingFrame = false;
};


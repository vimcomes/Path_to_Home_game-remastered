#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include "Fuel.h"
#include "GameState.h"
#include "Hud.h"
#include "Meteor.h"
#include "ParticleSystem.h"

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
    void updateBackground(float dt);
    void checkCollisions();
    void renderGameplay();
    void renderVictory();
    void renderPause();
    void renderPlayer();
    void showFinalGameOver();
    void initResources();
    void initWindowIcon();
    void initAudio();
    void loadConfig();
    void initBackground();
    void initPlayer();
    void initTexts();
    void setVictoryIfReached();
    void handleGameOverAnimation();
    void updateFuelGainText();

    sf::RenderWindow window;
    sf::View baseView{};

    game::Resources res{};
    game::Config cfg{};
    game::GameState state{};
    Hud hud{};
    ParticleSystem particles{};

    bool musicEnabled = true;
    sf::Music gameMusic;
    sf::Sound failSound;
    sf::Sound bonusSmallSound;
    sf::Sound bonusBigSound;
    sf::Sound victorySound;

    sf::RectangleShape pauseOverlay;
    sf::RectangleShape gamingBackground;
    sf::RectangleShape gamingBackground2;
    sf::RectangleShape bgFar1;
    sf::RectangleShape bgFar2;
    sf::RectangleShape vignette;
    sf::RectangleShape earth;

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

    bool thrustingFrame = false;
};

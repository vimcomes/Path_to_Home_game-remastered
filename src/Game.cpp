#include "Game.h"
#include "Fuel.h"
#include "Hud.h"
#include "Meteor.h"
#include "ParticleSystem.h"
#include "ResourcePaths.h"
#include "worklibgamer.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

Game::Game()
    : window(sf::VideoMode(1280, 720), "Path to home")
      , canister(resourcePath("images/galon.png"), 1000, 1000) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    initResources();
    initWindowIcon();
    initAudio();
    loadConfig();
    hud.initialize(res, cfg, state);
    initBackground();
    initPlayer();
    initTexts();
    baseView = window.getDefaultView();
}

void Game::run() {
    while (window.isOpen()) {
        const float dt = deltaClock.restart().asSeconds();
        update(dt);
        render();
    }
}

void Game::initResources() {
    if (!res.panel.loadFromFile(resourcePath("images/Panel.png")))
        throw std::runtime_error("Failed to load Panel.png");
    if (!res.background.loadFromFile(resourcePath("images/newkos1.jpg")))
        throw std::runtime_error("Failed to load background");
    if (!res.life.loadFromFile(resourcePath("images/playeranim1.png")))
        throw std::runtime_error("Failed to load life texture");
    if (!res.earth.loadFromFile(resourcePath("images/earth_PNG39.png")))
        throw std::runtime_error("Failed to load earth texture");
    if (!res.player.loadFromFile(resourcePath("images/playeranim1.png")))
        throw std::runtime_error("Failed to load player texture");
    if (!res.explosion.loadFromFile(resourcePath("images/bum.png")))
        throw std::runtime_error("Failed to load explosion texture");
    if (!res.font.loadFromFile(resourcePath("fonts/BankGothic Md BT Medium.otf")))
        throw std::runtime_error("Failed to load font");
    if (!res.sbFail.loadFromFile(resourcePath("audio/faled.wav")))
        throw std::runtime_error("Failed to load faled.wav");
    if (!res.sbBonusM.loadFromFile(resourcePath("audio/bonusm.wav")))
        throw std::runtime_error("Failed to load bonusm.wav");
    if (!res.sbBonusB.loadFromFile(resourcePath("audio/bonusB.wav")))
        throw std::runtime_error("Failed to load bonusB.wav");
    if (!res.sbVictory.loadFromFile(resourcePath("audio/victoria.wav")))
        throw std::runtime_error("Failed to load victoria.wav");
}

void Game::initWindowIcon() {
    sf::Image icon;
    if (icon.loadFromFile(resourcePath("images/icon.png"))) {
        window.setIcon(32, 32, icon.getPixelsPtr());
    }
}

void Game::initAudio() {
    musicEnabled = true;
    if (!gameMusic.openFromFile(resourcePath("audio/mgame.ogg")))
        throw std::runtime_error("Failed to load mgame.ogg");
    gameMusic.setLoop(true);
    gameMusic.setVolume(65.f);
    gameMusic.play();

    failSound.setBuffer(res.sbFail);
    bonusSmallSound.setBuffer(res.sbBonusM);
    bonusBigSound.setBuffer(res.sbBonusB);
    victorySound.setBuffer(res.sbVictory);
}

void Game::loadConfig() {
    const auto configFile = executableRelative("config.toml");
    std::error_code ec{};
    if (!std::filesystem::exists(configFile, ec)) {
        return; // defaults stay
    }

    std::ifstream in(configFile);
    if (!in.is_open())
        return;

    auto trim = [](std::string_view sv) {
        const auto isSpace = [](unsigned char c) { return std::isspace(c) != 0; };
        while (!sv.empty() && isSpace(sv.front()))
            sv.remove_prefix(1);
        while (!sv.empty() && isSpace(sv.back()))
            sv.remove_suffix(1);
        return sv;
    };

    std::unordered_map<std::string, double> values;
    std::string line;
    std::string currentSection;
    while (std::getline(in, line)) {
        auto sv = std::string_view(line);
        if (sv.empty() || sv.front() == '#')
            continue;
        if (sv.front() == '[' && sv.back() == ']') {
            currentSection = std::string(trim(sv.substr(1, sv.size() - 2)));
            continue;
        }
        auto pos = sv.find('=');
        if (pos == std::string_view::npos)
            continue;
        auto key = std::string(trim(sv.substr(0, pos)));
        auto valStr = trim(sv.substr(pos + 1));
        try {
            double val = std::stod(std::string(valStr));
            std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
            values[fullKey] = val;
        } catch (...) {
            continue;
        }
    }

    if (auto it = values.find("player.speed"); it != values.end())
        cfg.playerSpeed = static_cast<float>(it->second);
    if (auto it = values.find("background.speed"); it != values.end())
        cfg.backgroundSpeed = static_cast<float>(it->second);
    if (auto it = values.find("background.speed_far"); it != values.end())
        cfg.backgroundSpeedFar = static_cast<float>(it->second);
    if (auto it = values.find("progress.speed"); it != values.end())
        cfg.progressSpeed = static_cast<float>(it->second);
    if (auto it = values.find("fuel.max"); it != values.end())
        cfg.maxFuel = static_cast<float>(it->second);
    if (auto it = values.find("fuel.drain_per_second"); it != values.end())
        cfg.fuelDrainPerSecond = static_cast<float>(it->second);
    if (auto it = values.find("shield.duration"); it != values.end())
        cfg.shieldDuration = static_cast<float>(it->second);
}

void Game::initBackground() {
    gamingBackground = sf::RectangleShape(sf::Vector2f(1280.f, 720.f));
    gamingBackground.setTexture(&res.background);
    gamingBackground.setFillColor(sf::Color(255, 255, 255, 162));
    gamingBackground2 = sf::RectangleShape(sf::Vector2f(1280.f, 720.f));
    gamingBackground2.setTexture(&res.background);
    gamingBackground2.setFillColor(sf::Color(255, 255, 255, 162));
    gamingBackground2.setPosition(sf::Vector2f(1280.f, 0.f));

    bgFar1 = sf::RectangleShape(sf::Vector2f(1280.f, 720.f));
    bgFar1.setTexture(&res.background);
    bgFar1.setFillColor(sf::Color(255, 255, 255, 255));
    bgFar2 = sf::RectangleShape(sf::Vector2f(1280.f, 720.f));
    bgFar2.setTexture(&res.background);
    bgFar2.setPosition(sf::Vector2f(1280.f, 0.f));
    bgFar2.setFillColor(sf::Color(255, 255, 255, 255));

    pauseOverlay = sf::RectangleShape(sf::Vector2f(1280.f, 720.f));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    vignette = sf::RectangleShape(sf::Vector2f(1280.f, 720.f));
    vignette.setFillColor(sf::Color(0, 0, 0, 35));

    earth = sf::RectangleShape(sf::Vector2f(500.f, 500.f));
    earth.setTexture(&res.earth);
    earth.setPosition(sf::Vector2f(1100.f, 150.f));
}

void Game::initPlayer() {
    player.setTexture(res.player);
    player.setTextureRect(sf::IntRect(0, state.playerAnim.Frame, 90, 90));
    player.setScale(0.7f, 0.7f);
    player.setPosition(sf::Vector2f(80.f, 380.f));

    destruction.setTexture(res.explosion);
    destruction.setTextureRect(sf::IntRect(5, 15, 95, 80));
    destruction.setScale(0.7f, 0.7f);
}

void Game::initTexts() {
    textPause.setFont(res.font);
    textPause.setFillColor(sf::Color::Magenta);
    textPause.setCharacterSize(50);
    textPause.setString("P A U S E");
    textPause.setPosition(500.f, 333.f);

    winTitle.setFont(res.font);
    winTitle.setFillColor(sf::Color::Green);
    winTitle.setCharacterSize(72);
    winTitle.setString(L"Congratulations!");
    winTitle.setPosition(300.f, 240.f);

    winSub.setFont(res.font);
    winSub.setFillColor(sf::Color::White);
    winSub.setCharacterSize(28);
    winSub.setString(L"You successfully got home.");
    winSub.setPosition(360.f, 320.f);
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P)
                state.gamePause = !state.gamePause;
            if (event.key.code == sf::Keyboard::End)
                window.close();
            if (event.key.code == sf::Keyboard::M) {
                musicEnabled = !musicEnabled;
                if (musicEnabled)
                    gameMusic.play();
                else
                    gameMusic.stop();
            }
            if (event.key.code == sf::Keyboard::F1) {
                hud.setProgressToEnd();
                state.victoryPlayed = false;
            }
        }
    }
}

void Game::update(float dt) {
    state.deltaSeconds = dt;
    state.shakeTimer = std::max(0.f, state.shakeTimer - state.deltaSeconds);
    state.flashTimer = std::max(0.f, state.flashTimer - state.deltaSeconds);
    state.smokeAccumulator += state.deltaSeconds;
    state.shieldPulse += state.deltaSeconds;
    if (state.shieldActive) {
        state.shieldTimer -= state.deltaSeconds;
        if (state.shieldTimer <= 0.f)
            state.shieldActive = false;
    }
    baseView = window.getDefaultView();
    thrustingFrame = false;

    handleEvents();
    if (state.gamePause)
        return;

    if (hud.progressX() <= hud.progressEnd()) {
        updateGameplay(dt);
    } else {
        if (animTextClock.getElapsedTime() > sf::seconds(20))
            window.close();
    }
}

void Game::updateGameplay(float dt) {
    if (state.gameOver) {
        handleGameOverAnimation();
        return;
    }

    state.traffic = 0;
    state.moveRec = sf::Vector2f(0.f, 0.f);
    if (state.fuel > 0.f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(
                sf::Keyboard::Numpad5)) {
            state.moveRec.y += cfg.playerSpeed * dt;
            state.traffic = 2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(
                sf::Keyboard::Numpad8)) {
            state.moveRec.y -= cfg.playerSpeed * dt;
            state.traffic = 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(
                sf::Keyboard::Numpad4)) {
            state.moveRec.x -= cfg.playerSpeed * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(
                sf::Keyboard::Numpad6)) {
            state.moveRec.x += cfg.playerSpeed * dt;
        }
    }

    const bool thrusting = (state.moveRec.x != 0.f || state.moveRec.y != 0.f) && state.fuel > 0.f;
    if (thrusting) {
        thrustingFrame = true;
        if (!state.shieldActive)
            state.fuel = std::max(0.f, state.fuel - cfg.fuelDrainPerSecond * dt);
        if (state.smokeAccumulator >= 0.04f) {
            state.smokeAccumulator = 0.f;
            sf::FloatRect pb = player.getGlobalBounds();
            particles.emitSmoke(
                sf::Vector2f(
                    pb.left + pb.width * 0.18f + static_cast<float>(std::rand() % 4 - 2),
                    pb.top + pb.height * 0.50f + static_cast<float>(std::rand() % 4 - 2)),
                6.f + static_cast<float>(std::rand() % 5),
                0.45f);
        }
    }

    if (animPlayClock.getElapsedTime() > sf::milliseconds(100)) {
        animPlayClock.restart();
        playeranim(player, state.playerAnim, state.traffic);
    }
    if (animMeteorClock.getElapsedTime() > sf::milliseconds(80)) {
        animMeteorClock.restart();
        for (auto& m : meteors)
            m.animation();
    }
    updateFuelGainText();
    updateBackground(dt);

    PlayerMove(player, state.moveRec);
    particles.update(dt, cfg);
    checkCollisions();
    hud.update(state, cfg);
    setVictoryIfReached();
}

void Game::handleGameOverAnimation() {
    if (animMeteorClock.getElapsedTime() > sf::milliseconds(80)) {
        animMeteorClock.restart();
        state.destructAnim.Frame += state.destructAnim.Step;
        if (state.destructAnim.Frame > 405) {
            state.destructAnim.Frame1 += state.destructAnim.Step1;
            state.destructAnim.Frame = 5;
        }
        if (state.destructAnim.Frame1 > 415) {
            state.gameOver = false;
            player.setPosition(sf::Vector2f(80.f, 380.f));
            for (auto& m : meteors)
                m.restart();
            canister.restart();
            state.pusk = 0;
            state.destructAnim.Frame = 5;
            state.destructAnim.Frame1 = 15;
            state.fuel = 200.f;
            state.shieldActive = false;
            state.shieldTimer = 0.f;
            state.victoryPlayed = false;
            if (state.lives <= 0) {
                showFinalGameOver();
            }
        } else {
            destruction.setTextureRect(
                sf::IntRect(state.destructAnim.Frame, state.destructAnim.Frame1, 95, 80));
        }
    }
}

void Game::updateBackground(float dt) {
    bgFar1.move(-cfg.backgroundSpeedFar * dt, 0.f);
    sf::Vector2f pos = bgFar1.getPosition();
    if (pos.x < -1280)
        bgFar1.setPosition(1280.f, pos.y);
    bgFar2.move(-cfg.backgroundSpeedFar * dt, 0.f);
    pos = bgFar2.getPosition();
    if (pos.x < -1280)
        bgFar2.setPosition(1280.f, pos.y);

    gamingBackground.move(-cfg.backgroundSpeed * dt, 0.f);
    pos = gamingBackground.getPosition();
    if (pos.x < -1280)
        gamingBackground.setPosition(1280.f, pos.y);
    gamingBackground2.move(-cfg.backgroundSpeed * dt, 0.f);
    pos = gamingBackground2.getPosition();
    if (pos.x < -1280)
        gamingBackground2.setPosition(1280.f, pos.y);

    hud.moveProgress(cfg.progressSpeed * dt);
}

void Game::checkCollisions() {
    const sf::FloatRect playerHitbox = ShrinkRect(player.getGlobalBounds(), 0.18f);

    for (std::size_t i = 0; i < meteors.size(); ++i) {
        if (meteors[i].newborn)
            Correct(canister, static_cast<int>(i), meteors.data(),
                    static_cast<int>(meteors.size()));
        meteors[i].move(state.deltaSeconds);
        sf::FloatRect meteorHitbox = ShrinkRect(meteors[i].getMeteorBounds(), 0.12f);
        if (meteorHitbox.intersects(playerHitbox)) {
            if (state.shieldActive) {
                sf::Vector2f hitPos = meteors[i].getPosBonus();
                meteors[i].restart();
                particles.emitFlash(hitPos, 0.25f);
            } else {
                state.lives = std::max(0, state.lives - 1);
                failSound.play();
                state.gameOver = true;
                destruction.setPosition(player.getPosition().x, player.getPosition().y);
                state.shakeTimer = 0.35f;
                state.flashTimer = 0.25f;
                break;
            }
        }
    }

    if (canister.newborn)
        CorrectFuel(canister, meteors.data(), static_cast<int>(meteors.size()));
    canister.move(state.deltaSeconds);

    if (ShrinkRect(canister.getMeteorBounds(), 0.12f).intersects(playerHitbox)) {
        if (canister.getType() == BonusType::Shield) {
            state.shieldActive = true;
            state.shieldTimer = cfg.shieldDuration;
            bonusBigSound.play();
            hud.setFuelGainShieldText(canister.getPosBonus());
            state.pusk = 30;
            particles.emitFlash(player.getPosition(), 0.25f);
        } else {
            state.fuelGain = canister.getAmount();
            state.fuel = std::min(cfg.maxFuel, state.fuel + static_cast<float>(state.fuelGain));
            if (state.fuelGain > 50)
                bonusBigSound.play();
            else
                bonusSmallSound.play();
            hud.setFuelGainAmountText(IntToStr(static_cast<int>(state.fuelGain)));
            state.pusk = 40;
        }
        canister.restart();
        particles.emitFlash(player.getPosition(), 0.25f);
    }
}

void Game::render() {
    if (state.gamePause) {
        renderPause();
    } else if (hud.progressX() <= hud.progressEnd()) {
        renderGameplay();
    } else {
        renderVictory();
    }
}

void Game::renderGameplay() {
    window.clear();
    sf::View view = baseView;
    if (state.shakeTimer > 0.f) {
        float strength = 6.f * (state.shakeTimer / 0.35f);
        float ox = (static_cast<float>(std::rand() % 200) / 100.f - 1.f) * strength;
        float oy = (static_cast<float>(std::rand() % 200) / 100.f - 1.f) * strength;
        view.move(ox, oy);
    }
    window.setView(view);

    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    window.draw(vignette);
    hud.draw(window, state.lives);
    particles.drawSmoke(window);
    particles.drawFlashes(window);

    if (thrustingFrame) {
        sf::FloatRect pb = player.getGlobalBounds();
        sf::CircleShape glow(18.f);
        glow.setFillColor(sf::Color(255, 200, 80, 160));
        glow.setPosition(pb.left + pb.width * 0.1f, pb.top + pb.height * 0.42f);
        window.draw(glow);
    }

    renderPlayer();
    for (auto& m : meteors)
        m.draw(window);
    canister.draw(window);
    hud.drawFuelGain(window, state.pusk);
    if (state.flashTimer > 0.f) {
        sf::RectangleShape flash(sf::Vector2f(1280.f, 720.f));
        flash.setFillColor(sf::Color(255, 255, 255,
                                     static_cast<sf::Uint8>(255 * (state.flashTimer / 0.25f))));
        window.draw(flash);
    }
    window.display();
    setVictoryIfReached();
}

void Game::renderVictory() {
    window.clear();
    window.setView(baseView);
    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    window.draw(vignette);
    hud.draw(window, state.lives);
    window.draw(earth);
    window.draw(winTitle);
    window.draw(winSub);
    window.display();
}

void Game::renderPause() {
    window.clear();
    window.setView(baseView);
    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    hud.draw(window, state.lives);
    window.draw(pauseOverlay);
    window.draw(textPause);
    window.display();
}

void Game::renderPlayer() {
    if (state.gameOver) {
        window.draw(destruction);
        return;
    }

    const float outlineScale = 1.2f;
    auto lb = player.getLocalBounds();
    sf::Vector2f center(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    sf::Vector2f worldCenter(player.getPosition().x + player.getGlobalBounds().width / 2.f,
                             player.getPosition().y + player.getGlobalBounds().height / 2.f);
    sf::Sprite outline = player;
    outline.setOrigin(center);
    outline.setPosition(worldCenter);
    outline.setScale(player.getScale().x * outlineScale, player.getScale().y * outlineScale);
    outline.setColor(sf::Color(100, 180, 255, 190));
    sf::RenderStates addBlend;
    addBlend.blendMode = sf::BlendAdd;
    if (state.shieldActive) {
        float pulse = 0.5f + 0.5f * std::sin(state.shieldPulse * 10.f);
        outline.setColor(sf::Color(255, 60, 60, static_cast<sf::Uint8>(140 + 100 * pulse)));
        window.draw(outline, addBlend);
    } else {
        window.draw(outline, addBlend);
    }
    if (state.shieldActive) {
        float ringRadius = std::max(player.getGlobalBounds().width, player.getGlobalBounds().height)
                           * 0.8f;
        sf::CircleShape ring(ringRadius);
        ring.setOrigin(ringRadius, ringRadius);
        ring.setPosition(player.getPosition().x + player.getGlobalBounds().width / 2.f,
                         player.getPosition().y + player.getGlobalBounds().height / 2.f);
        float pulse = 0.5f + 0.5f * std::sin(state.shieldPulse * 12.f);
        ring.setOutlineThickness(3.f);
        ring.setOutlineColor(sf::Color(255, 80, 80, static_cast<sf::Uint8>(120 + 80 * pulse)));
        ring.setFillColor(sf::Color(255, 50, 50, static_cast<sf::Uint8>(40 * pulse)));
        window.draw(ring, addBlend);
    }
    window.draw(player);
}

void Game::setVictoryIfReached() {
    if (hud.progressX() >= hud.progressEnd() && !state.victoryPlayed) {
        hud.setProgressToEnd();
        gameMusic.stop();
        victorySound.play();
        state.victoryPlayed = true;
    }
}

void Game::updateFuelGainText() {
    if (animTextClock.getElapsedTime() > sf::milliseconds(50)) {
        animTextClock.restart();
        hud.animateFuelGain(state.pusk);
    }
}

void Game::showFinalGameOver() {
    window.clear();
    window.setView(baseView);
    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    hud.draw(window, state.lives);

    sf::Text finalMsg;
    sf::Text hintMsg;
    finalMsg.setFont(res.font);
    finalMsg.setCharacterSize(80);
    finalMsg.setFillColor(sf::Color::Red);
    finalMsg.setString(L"GAME OVER");
    finalMsg.setPosition(380.f, 280.f);
    hintMsg.setFont(res.font);
    hintMsg.setCharacterSize(30);
    hintMsg.setFillColor(sf::Color::White);
    hintMsg.setString(L"Press any key to restart");
    hintMsg.setPosition(400.f, 380.f);
    window.draw(finalMsg);
    window.draw(hintMsg);
    window.display();

    bool waitKey = true;
    sf::Event ev;
    while (waitKey && window.isOpen()) {
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (ev.type == sf::Event::KeyPressed)
                waitKey = false;
        }
    }
    state.lives = 5;
    hud.resetProgress();
    canister.restart();
    particles.clear();
}

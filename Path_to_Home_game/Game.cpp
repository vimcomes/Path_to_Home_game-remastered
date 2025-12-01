#include "Game.h"
#include "worklibgamer.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

Game::Game()
    : window(sf::VideoMode(1280, 720), "Path to home")
    , canister("Image/galon.png", 1000, 1000)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    initResources();
    initWindowIcon();
    initAudio();
    initHUD();
    initBackground();
    initPlayer();
    initTexts();
    baseView = window.getDefaultView();
}

void Game::run()
{
    while (window.isOpen())
    {
        const float dt = deltaClock.restart().asSeconds();
        update(dt);
        render();
    }
}

void Game::initResources()
{
    if (!res.panel.loadFromFile("Image/Panel.png")) throw std::runtime_error("Failed to load Panel.png");
    if (!res.background.loadFromFile("Image/newkos1.jpg")) throw std::runtime_error("Failed to load background");
    if (!res.life.loadFromFile("Image/playeranim1.png")) throw std::runtime_error("Failed to load life texture");
    if (!res.earth.loadFromFile("Image/earth_PNG39.png")) throw std::runtime_error("Failed to load earth texture");
    if (!res.player.loadFromFile("Image/playeranim1.png")) throw std::runtime_error("Failed to load player texture");
    if (!res.explosion.loadFromFile("Image/bum.png")) throw std::runtime_error("Failed to load explosion texture");
    if (!res.font.loadFromFile("Font/BankGothic Md BT Medium.otf")) throw std::runtime_error("Failed to load font");
    if (!res.sbFail.loadFromFile("Music/faled.wav")) throw std::runtime_error("Failed to load faled.wav");
    if (!res.sbBonusM.loadFromFile("Music/bonusm.wav")) throw std::runtime_error("Failed to load bonusm.wav");
    if (!res.sbBonusB.loadFromFile("Music/bonusB.wav")) throw std::runtime_error("Failed to load bonusB.wav");
    if (!res.sbVictory.loadFromFile("Music/victoria.wav")) throw std::runtime_error("Failed to load victoria.wav");
}

void Game::initWindowIcon()
{
    sf::Image icon;
    if (icon.loadFromFile("Image/icon.png"))
    {
        window.setIcon(32, 32, icon.getPixelsPtr());
    }
}

void Game::initAudio()
{
    musicEnabled = true;
    if (!gameMusic.openFromFile("Music/mgame.ogg")) throw std::runtime_error("Failed to load mgame.ogg");
    gameMusic.setLoop(true);
    gameMusic.setVolume(65.f);
    gameMusic.play();

    failSound.setBuffer(res.sbFail);
    bonusSmallSound.setBuffer(res.sbBonusM);
    bonusBigSound.setBuffer(res.sbBonusB);
    victorySound.setBuffer(res.sbVictory);
}

void Game::initHUD()
{
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

    progressStart = progressBg.getPosition().x;
    progressEnd = progressBg.getPosition().x + progressBg.getSize().x;
    progressMarkerY = progressBg.getPosition().y + progressBg.getSize().y / 2.f;
    progressMarker.setPosition(progressStart, progressMarkerY);

    initLifeIcons();
}

void Game::initBackground()
{
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

void Game::initPlayer()
{
    player.setTexture(res.player);
    player.setTextureRect(sf::IntRect(0, state.playerAnim.Frame, 90, 90));
    player.setScale(0.7f, 0.7f);
    player.setPosition(sf::Vector2f(80.f, 380.f));

    destruction.setTexture(res.explosion);
    destruction.setTextureRect(sf::IntRect(5, 15, 95, 80));
    destruction.setScale(0.7f, 0.7f);
}

void Game::initTexts()
{
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
    fuelMax.setPosition(fuelBg.getPosition().x + fuelBg.getSize().x + 6.f, fuelBg.getPosition().y - 2.f);

    fuelGainText.setFont(res.font);
    fuelGainText.setFillColor(sf::Color::Green);
    fuelGainText.setCharacterSize(25);
    fuelGainText.setOutlineColor(sf::Color(0, 0, 0, 160));
    fuelGainText.setOutlineThickness(1.f);

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

void Game::initLifeIcons()
{
    hud.lifeIcons.clear();
    sf::Sprite lifeSprite(res.life);
    lifeSprite.setTextureRect(sf::IntRect(0, 700, 90, 90));
    lifeSprite.setScale(0.25f, 0.25f);
    for (int i = 0; i < state.lives; ++i)
    {
        sf::Sprite icon = lifeSprite;
        icon.setPosition(30.f + i * 50.f, 75.f);
        hud.lifeIcons.push_back(icon);
    }
}

void Game::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::P) state.gamePause = !state.gamePause;
            if (event.key.code == sf::Keyboard::End) window.close();
            if (event.key.code == sf::Keyboard::M)
            {
                musicEnabled = !musicEnabled;
                if (musicEnabled) gameMusic.play(); else gameMusic.stop();
            }
            if (event.key.code == sf::Keyboard::F1)
            {
                progressMarker.setPosition(progressEnd, progressMarker.getPosition().y);
                state.victoryPlayed = false;
            }
        }
    }
}

void Game::update(float dt)
{
    state.deltaSeconds = dt;
    state.shakeTimer = std::max(0.f, state.shakeTimer - state.deltaSeconds);
    state.flashTimer = std::max(0.f, state.flashTimer - state.deltaSeconds);
    state.smokeAccumulator += state.deltaSeconds;
    state.shieldPulse += state.deltaSeconds;
    if (state.shieldActive)
    {
        state.shieldTimer -= state.deltaSeconds;
        if (state.shieldTimer <= 0.f) state.shieldActive = false;
    }
    baseView = window.getDefaultView();
    thrustingFrame = false;

    handleEvents();
    if (state.gamePause) return;

    if (progressMarker.getPosition().x <= progressEnd)
    {
        updateGameplay(dt);
    }
    else
    {
        if (animTextClock.getElapsedTime() > sf::seconds(20)) window.close();
    }
}

void Game::updateGameplay(float dt)
{
    if (state.gameOver)
    {
        handleGameOverAnimation();
        return;
    }

    state.traffic = 0;
    state.moveRec = sf::Vector2f(0.f, 0.f);
    if (state.fuel > 0.f)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
        {
            state.moveRec.y += cfg.playerSpeed * dt;
            state.traffic = 2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8))
        {
            state.moveRec.y -= cfg.playerSpeed * dt;
            state.traffic = 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4))
        {
            state.moveRec.x -= cfg.playerSpeed * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6))
        {
            state.moveRec.x += cfg.playerSpeed * dt;
        }
    }

    const bool thrusting = (state.moveRec.x != 0.f || state.moveRec.y != 0.f) && state.fuel > 0.f;
    if (thrusting)
    {
        thrustingFrame = true;
        if (!state.shieldActive)
            state.fuel = std::max(0.f, state.fuel - cfg.fuelDrainPerSecond * dt);
        if (state.smokeAccumulator >= 0.04f)
        {
            state.smokeAccumulator = 0.f;
            sf::FloatRect pb = player.getGlobalBounds();
            game::SmokeParticle p;
            p.pos = sf::Vector2f(pb.left + pb.width * 0.18f + static_cast<float>(std::rand() % 4 - 2),
                                 pb.top + pb.height * 0.50f + static_cast<float>(std::rand() % 4 - 2));
            p.ttl = 0.45f;
            p.lifetime = p.ttl;
            p.size = 6.f + static_cast<float>(std::rand() % 5);
            state.smoke.push_back(p);
        }
    }

    if (animPlayClock.getElapsedTime() > sf::milliseconds(100))
    {
        animPlayClock.restart();
        playeranim(player, state.playerAnim, state.traffic);
    }
    if (animMeteorClock.getElapsedTime() > sf::milliseconds(80))
    {
        animMeteorClock.restart();
        for (auto& m : meteors) m.animation();
    }
    updateFuelGainText();
    updateBackground(dt);

    PlayerMove(player, state.moveRec);
    updateParticles(dt);
    checkCollisions();
    updateHUD();
    setVictoryIfReached();
}

void Game::handleGameOverAnimation()
{
    if (animMeteorClock.getElapsedTime() > sf::milliseconds(80))
    {
        animMeteorClock.restart();
        state.destructAnim.Frame += state.destructAnim.Step;
        if (state.destructAnim.Frame > 405)
        {
            state.destructAnim.Frame1 += state.destructAnim.Step1;
            state.destructAnim.Frame = 5;
        }
        if (state.destructAnim.Frame1 > 415)
        {
            state.gameOver = false;
            player.setPosition(sf::Vector2f(80.f, 380.f));
            for (auto& m : meteors) m.restart();
            canister.restart();
            state.pusk = 0;
            state.destructAnim.Frame = 5;
            state.destructAnim.Frame1 = 15;
            state.fuel = 200.f;
            state.shieldActive = false;
            state.shieldTimer = 0.f;
            state.victoryPlayed = false;
            if (state.lives <= 0)
            {
                showFinalGameOver();
            }
        }
        else
        {
            destruction.setTextureRect(sf::IntRect(state.destructAnim.Frame, state.destructAnim.Frame1, 95, 80));
        }
    }
}

void Game::updateParticles(float dt)
{
    for (auto it = state.smoke.begin(); it != state.smoke.end(); )
    {
        it->lifetime -= dt;
        it->pos.x -= cfg.backgroundSpeed * dt * 0.5f;
        it->pos.y -= 10.f * dt;
        if (it->lifetime <= 0.f) it = state.smoke.erase(it);
        else ++it;
    }

    for (auto it = state.flashes.begin(); it != state.flashes.end(); )
    {
        it->lifetime -= dt;
        if (it->lifetime <= 0.f) it = state.flashes.erase(it);
        else ++it;
    }
}

void Game::updateBackground(float dt)
{
    bgFar1.move(-cfg.backgroundSpeedFar * dt, 0.f);
    sf::Vector2f pos = bgFar1.getPosition();
    if (pos.x < -1280) bgFar1.setPosition(1280.f, pos.y);
    bgFar2.move(-cfg.backgroundSpeedFar * dt, 0.f);
    pos = bgFar2.getPosition();
    if (pos.x < -1280) bgFar2.setPosition(1280.f, pos.y);

    gamingBackground.move(-cfg.backgroundSpeed * dt, 0.f);
    pos = gamingBackground.getPosition();
    if (pos.x < -1280) gamingBackground.setPosition(1280.f, pos.y);
    gamingBackground2.move(-cfg.backgroundSpeed * dt, 0.f);
    pos = gamingBackground2.getPosition();
    if (pos.x < -1280) gamingBackground2.setPosition(1280.f, pos.y);

    progressMarker.move(cfg.progressSpeed * dt, 0.f);
}

void Game::checkCollisions()
{
    const sf::FloatRect playerHitbox = ShrinkRect(player.getGlobalBounds(), 0.18f);

    for (std::size_t i = 0; i < meteors.size(); ++i)
    {
        if (meteors[i].newborn) Correct(canister, static_cast<int>(i), meteors.data(), static_cast<int>(meteors.size()));
        meteors[i].move(state.deltaSeconds);
        sf::FloatRect meteorHitbox = ShrinkRect(meteors[i].getMeteorBounds(), 0.12f);
        if (meteorHitbox.intersects(playerHitbox))
        {
            if (state.shieldActive)
            {
                sf::Vector2f hitPos = meteors[i].getPosBonus();
                meteors[i].restart();
                state.flashes.push_back(game::PickupFlash{ hitPos, 0.25f, 0.25f });
            }
            else
            {
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

    if (canister.newborn) CorrectFuel(canister, meteors.data(), static_cast<int>(meteors.size()));
    canister.move(state.deltaSeconds);

    if (ShrinkRect(canister.getMeteorBounds(), 0.12f).intersects(playerHitbox))
    {
        if (canister.getType() == BonusType::Shield)
        {
            state.shieldActive = true;
            state.shieldTimer = cfg.shieldDuration;
            bonusBigSound.play();
            fuelGainText.setString(L"SHIELD");
            fuelGainText.setFillColor(sf::Color::Red);
            fuelGainText.setPosition(canister.getPosBonus().x, canister.getPosBonus().y);
            state.pusk = 30;
            state.flashes.push_back(game::PickupFlash{ player.getPosition(), 0.25f, 0.25f });
        }
        else
        {
            fuelGainText.setFillColor(sf::Color::Green);
            state.fuelGain = canister.getAmount();
            state.fuel = std::min(cfg.maxFuel, state.fuel + static_cast<float>(state.fuelGain));
            if (state.fuelGain > 50) bonusBigSound.play(); else bonusSmallSound.play();
            fuelGainText.setString(IntToStr(static_cast<int>(state.fuelGain)));
            fuelGainText.setPosition(fuelBg.getPosition().x + fuelBg.getSize().x - 30.f, fuelBg.getPosition().y + 22.f);
            state.pusk = 40;
        }
        canister.restart();
        state.flashes.push_back(game::PickupFlash{ player.getPosition(), 0.25f, 0.25f });
    }
}

void Game::updateHUD()
{
    float fuelPercent = std::clamp(state.fuel / cfg.maxFuel, 0.f, 1.f);
    float fuelWidth = fuelBg.getSize().x * fuelPercent;
    fuelFill.setSize(sf::Vector2f(fuelWidth, fuelBg.getSize().y));
    fuelShine.setSize(sf::Vector2f(fuelWidth, fuelShine.getSize().y));
    if (fuelPercent >= 0.5f) fuelFill.setFillColor(sf::Color::Green);
    else if (fuelPercent >= 0.25f) fuelFill.setFillColor(sf::Color::Yellow);
    else fuelFill.setFillColor(sf::Color::Red);

    float progressPercent = std::clamp((progressMarker.getPosition().x - progressStart) / (progressEnd - progressStart), 0.f, 1.f);
    float progressWidth = progressBg.getSize().x * progressPercent;
    progressFill.setSize(sf::Vector2f(progressWidth, progressBg.getSize().y));
    progressShine.setSize(sf::Vector2f(progressWidth, progressShine.getSize().y));
}

void Game::render()
{
    if (state.gamePause)
    {
        renderPause();
    }
    else if (progressMarker.getPosition().x <= progressEnd)
    {
        renderGameplay();
    }
    else
    {
        renderVictory();
    }
}

void Game::renderGameplay()
{
    window.clear();
    sf::View view = baseView;
    if (state.shakeTimer > 0.f)
    {
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
    drawHUD();
    renderSmoke();
    renderFlashes();

    if (thrustingFrame)
    {
        sf::FloatRect pb = player.getGlobalBounds();
        sf::CircleShape glow(18.f);
        glow.setFillColor(sf::Color(255, 200, 80, 160));
        glow.setPosition(pb.left + pb.width * 0.1f, pb.top + pb.height * 0.42f);
        window.draw(glow);
    }

    renderPlayer();
    for (auto& m : meteors) m.draw(window);
    canister.draw(window);
    if (state.pusk > 0) window.draw(fuelGainText);
    if (state.flashTimer > 0.f)
    {
        sf::RectangleShape flash(sf::Vector2f(1280.f, 720.f));
        flash.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * (state.flashTimer / 0.25f))));
        window.draw(flash);
    }
    window.display();
    setVictoryIfReached();
}

void Game::renderVictory()
{
    window.clear();
    window.setView(baseView);
    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    window.draw(vignette);
    drawHUD();
    window.draw(earth);
    window.draw(winTitle);
    window.draw(winSub);
    window.display();
}

void Game::renderPause()
{
    window.clear();
    window.setView(baseView);
    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    window.draw(gameInfoPanel);
    window.draw(progressBg);
    window.draw(progressFill);
    window.draw(progressShine);
    window.draw(progressLabel);
    window.draw(fuelBg);
    window.draw(fuelFill);
    window.draw(fuelShine);
    window.draw(fuelLabel);
    drawHUD();
    window.draw(pauseOverlay);
    window.draw(textPause);
    window.display();
}

void Game::renderSmoke()
{
    for (const auto& s : state.smoke)
    {
        sf::CircleShape puff(s.size);
        float alpha = static_cast<float>(std::max(0.f, s.lifetime)) / s.ttl;
        float faded = std::pow(alpha, 0.8f);
        puff.setFillColor(sf::Color(
            230,
            230,
            230,
            static_cast<sf::Uint8>(170 * faded)));
        puff.setPosition(s.pos);
        window.draw(puff);
    }
}

void Game::renderFlashes()
{
    for (const auto& f : state.flashes)
    {
        float k = f.lifetime / f.ttl;
        float radius = 10.f + 25.f * (1.f - k);
        sf::CircleShape flash(radius);
        flash.setOrigin(radius, radius);
        flash.setPosition(f.pos);
        flash.setFillColor(sf::Color(255, 200, 80, static_cast<sf::Uint8>(190 * k)));
        window.draw(flash);
        sf::CircleShape flashCore(radius * 0.5f);
        flashCore.setOrigin(radius * 0.5f, radius * 0.5f);
        flashCore.setPosition(f.pos);
        flashCore.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(200 * k)));
        window.draw(flashCore);
    }
}

void Game::renderPlayer()
{
    if (state.gameOver)
    {
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
    if (state.shieldActive)
    {
        float pulse = 0.5f + 0.5f * std::sin(state.shieldPulse * 10.f);
        outline.setColor(sf::Color(255, 60, 60, static_cast<sf::Uint8>(140 + 100 * pulse)));
        window.draw(outline, addBlend);
    }
    else
    {
        window.draw(outline, addBlend);
    }
    if (state.shieldActive)
    {
        float ringRadius = std::max(player.getGlobalBounds().width, player.getGlobalBounds().height) * 0.8f;
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

void Game::drawHUD()
{
    window.draw(gameInfoPanel);
    window.draw(progressBg);
    window.draw(progressFill);
    window.draw(progressShine);
    window.draw(progressLabel);
    window.draw(fuelBg);
    window.draw(fuelFill);
    window.draw(fuelShine);
    window.draw(fuelLabel);
    window.draw(fuelMax);
    for (int i = 0; i < state.lives && i < static_cast<int>(hud.lifeIcons.size()); ++i) window.draw(hud.lifeIcons[i]);
    progressMarkerGlow.setPosition(progressMarker.getPosition().x - progressMarkerGlow.getRadius() + progressMarker.getRadius(),
        progressMarker.getPosition().y - progressMarkerGlow.getRadius() + progressMarker.getRadius());
    window.draw(progressMarkerGlow);
    window.draw(progressMarker);
}

void Game::setVictoryIfReached()
{
    if (progressMarker.getPosition().x >= progressEnd && !state.victoryPlayed)
    {
        progressMarker.setPosition(progressEnd, progressMarker.getPosition().y);
        gameMusic.stop();
        victorySound.play();
        state.victoryPlayed = true;
    }
}

void Game::updateFuelGainText()
{
    if (animTextClock.getElapsedTime() > sf::milliseconds(50))
    {
        animTextClock.restart();
        if (state.pusk > 0)
        {
            state.pusk--;
            fuelGainText.move(0.f, -1.f);
        }
    }
}

void Game::showFinalGameOver()
{
    window.clear();
    window.setView(baseView);
    window.draw(bgFar2);
    window.draw(bgFar1);
    window.draw(gamingBackground2);
    window.draw(gamingBackground);
    drawHUD();

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
    while (waitKey && window.isOpen())
    {
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            if (ev.type == sf::Event::KeyPressed) waitKey = false;
        }
    }
    state.lives = 5;
    progressMarker.setPosition(progressStart, progressMarkerY);
    canister.restart();
}

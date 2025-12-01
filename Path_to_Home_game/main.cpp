#include "worklibgamer.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

struct SmokeParticle
{
    Vector2f pos;
    float lifetime;
    float ttl;
    float size;
};

struct PickupFlash
{
    Vector2f pos;
    float lifetime;
    float ttl;
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
    Vector2f moveRec{};
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

struct Resources {
    Texture panel;
    Texture background;
    Texture life;
    Texture earth;
    Texture player;
    Texture explosion;
    Font font;
    SoundBuffer sbFail, sbBonusM, sbBonusB, sbVictory;
};

struct HudSprites
{
    std::vector<Sprite> lifeIcons;
};

Resources res;
Config cfg;
GameState state;
HudSprites hud;

// ------------------------ Helpers ------------------------
void handleEvents(sf::RenderWindow& win, GameState& state, sf::Music& gameMusic, bool& mus, CircleShape& Rcircle, float progressEnd)
{
    sf::Event event;
    while (win.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) win.close();
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::P) state.gamePause = !state.gamePause;
            if (event.key.code == sf::Keyboard::End) win.close();
            if (event.key.code == sf::Keyboard::M)
            {
                mus = !mus;
                if (mus) gameMusic.play(); else gameMusic.stop();
            }
            if (event.key.code == sf::Keyboard::F1)
            {
                // Debug: быстрое завершение пути
                Rcircle.setPosition(progressEnd, Rcircle.getPosition().y);
                state.victoryPlayed = false;
            }
        }
    }
}

void drawHUD(GameState& state,
             sf::RenderWindow& win,
             RectangleShape& GameInfoPanel,
             RectangleShape& progressBg, RectangleShape& progressFill, RectangleShape& progressShine, Text& progressLabel,
             RectangleShape& fuelBg, RectangleShape& fuelFill, RectangleShape& fuelShine, Text& fuelLabel, Text& fuelMax,
             const std::vector<Sprite>& lifeIcons,
             CircleShape& Rcircle, CircleShape& RcircleGlow)
{
    win.draw(GameInfoPanel);
    win.draw(progressBg);
    win.draw(progressFill);
    win.draw(progressShine);
    win.draw(progressLabel);
    win.draw(fuelBg);
    win.draw(fuelFill);
    win.draw(fuelShine);
    win.draw(fuelLabel);
    win.draw(fuelMax);
    for (int i = 0; i < state.lives && i < static_cast<int>(lifeIcons.size()); ++i) win.draw(lifeIcons[i]);
    RcircleGlow.setPosition(Rcircle.getPosition().x - RcircleGlow.getRadius() + Rcircle.getRadius(),
        Rcircle.getPosition().y - RcircleGlow.getRadius() + Rcircle.getRadius());
    win.draw(RcircleGlow);
    win.draw(Rcircle);
}

int main()
{
    srand(time(NULL));  

    // Base window and icon setup
    RenderWindow win(VideoMode(1280, 720), "Path to home");
    Image icon;
    if (!icon.loadFromFile("Image/icon.png")) return 13; 
    win.setIcon(32, 32, icon.getPixelsPtr());
    
    // Music and sounds
    bool mus = true; 
    Music game_music;
    if (!game_music.openFromFile("Music/mgame.ogg")) return 14; 
    game_music.setLoop(true);
    game_music.setVolume(65.f);
    game_music.play();

    Sound falsound, soundbonM, soundbonB, soundvic;

    res.panel.loadFromFile("Image/Panel.png");
    res.background.loadFromFile("Image/newkos1.jpg");
    res.life.loadFromFile("Image/playeranim1.png");
    res.earth.loadFromFile("Image/earth_PNG39.png");
    res.player.loadFromFile("Image/playeranim1.png");
    res.explosion.loadFromFile("Image/bum.png");
    res.font.loadFromFile("Font/BankGothic Md BT Medium.otf");
    res.sbFail.loadFromFile("Music/faled.wav");
    res.sbBonusM.loadFromFile("Music/bonusm.wav");
    res.sbBonusB.loadFromFile("Music/bonusB.wav");
    res.sbVictory.loadFromFile("Music/victoria.wav");
    falsound.setBuffer(res.sbFail);
    soundbonM.setBuffer(res.sbBonusM);
    soundbonB.setBuffer(res.sbBonusB);
    soundvic.setBuffer(res.sbVictory);

    RectangleShape GameInfoPanel(Vector2f(1280, 113));
    GameInfoPanel.setTexture(&res.panel);
    GameInfoPanel.setPosition(Vector2f(0, 0));
    
    CircleShape Rcircle(5.f);
    Rcircle.setFillColor(Color(255, 0, 0));       
    Rcircle.setOutlineThickness(2.f);             
    Rcircle.setOutlineColor(Color(255, 155, 0));  
    CircleShape RcircleGlow(11.f);
    RcircleGlow.setFillColor(Color(255, 180, 50, 120));
    
    Text fuelGainText, text_pause, end_game;
    Font GameFont = res.font;
    
    fuelGainText.setFont(GameFont);
    fuelGainText.setFillColor(Color::Green);
    fuelGainText.setCharacterSize(25);
    fuelGainText.setOutlineColor(Color(0, 0, 0, 160));
    fuelGainText.setOutlineThickness(1.f);
    
    end_game.setFont(GameFont);
    end_game.setFillColor(Color::Red);
    end_game.setCharacterSize(100);
    end_game.setString(L"GAME OVER");
    end_game.setPosition(300, 333);
    Text win_title;
    win_title.setFont(res.font);
    win_title.setFillColor(Color::Green);
    win_title.setCharacterSize(72);
    win_title.setString(L"Congratulations!");
    win_title.setPosition(300.f, 240.f);
    Text win_sub;
    win_sub.setFont(res.font);
    win_sub.setFillColor(Color::White);
    win_sub.setCharacterSize(28);
    win_sub.setString(L"You successfully got home.");
    win_sub.setPosition(360.f, 320.f);
    
    text_pause.setFont(GameFont);
    text_pause.setFillColor(Color::Magenta);
    text_pause.setCharacterSize(50);
    text_pause.setString("P A U S E");
    text_pause.setPosition(500, 333);

    // Полосы прогресса и топлива
    RectangleShape progressBg(Vector2f(300.f, 14.f));
    progressBg.setFillColor(Color(40, 40, 40, 200));
    progressBg.setOutlineColor(Color(10, 10, 10, 200));
    progressBg.setOutlineThickness(1.5f);
    progressBg.setPosition(520.f, 60.f);
    RectangleShape progressFill(Vector2f(0.f, 14.f));
    progressFill.setFillColor(Color(100, 210, 255));
    progressFill.setPosition(progressBg.getPosition());
    RectangleShape progressShine(Vector2f(0.f, 6.f));
    progressShine.setFillColor(Color(180, 230, 255, 140));
    progressShine.setPosition(progressBg.getPosition().x, progressBg.getPosition().y + 4.f);

    RectangleShape fuelBg(Vector2f(220.f, 14.f));
    fuelBg.setFillColor(Color(40, 40, 40, 200));
    fuelBg.setOutlineColor(Color(10, 10, 10, 200));
    fuelBg.setOutlineThickness(1.5f);
    fuelBg.setPosition(1040.f, 60.f);
    RectangleShape fuelFill(Vector2f(0.f, 14.f));
    fuelFill.setFillColor(Color::Green);
    fuelFill.setPosition(fuelBg.getPosition());
    RectangleShape fuelShine(Vector2f(0.f, 6.f));
    fuelShine.setFillColor(Color(200, 255, 200, 140));
    fuelShine.setPosition(fuelBg.getPosition().x, fuelBg.getPosition().y + 4.f);

    Text progressLabel, fuelLabel;
    progressLabel.setFont(GameFont);
    progressLabel.setCharacterSize(18);
    progressLabel.setFillColor(Color(230, 230, 230));
    progressLabel.setOutlineColor(Color(0, 0, 0, 130));
    progressLabel.setOutlineThickness(1.f);
    progressLabel.setString(L"Прогресс");
    progressLabel.setPosition(progressBg.getPosition().x - 120.f, progressBg.getPosition().y - 4.f);

    fuelLabel.setFont(GameFont);
    fuelLabel.setCharacterSize(18);
    fuelLabel.setFillColor(Color(230, 230, 230));
    fuelLabel.setOutlineColor(Color(0, 0, 0, 130));
    fuelLabel.setOutlineThickness(1.f);
    fuelLabel.setString(L"Топливо");
    fuelLabel.setPosition(fuelBg.getPosition().x - 100.f, fuelBg.getPosition().y - 4.f);

    Text fuelMax;
    fuelMax.setFont(GameFont);
    fuelMax.setCharacterSize(14);
    fuelMax.setFillColor(Color(220, 220, 220));
    fuelMax.setOutlineColor(Color(0, 0, 0, 130));
    fuelMax.setOutlineThickness(1.f);
    fuelMax.setString(L"MAX");
    fuelMax.setPosition(fuelBg.getPosition().x + fuelBg.getSize().x + 6.f, fuelBg.getPosition().y - 2.f);

    Rcircle.setPosition(progressBg.getPosition().x, progressBg.getPosition().y + progressBg.getSize().y / 2.f);

    Sprite lifeSprite(res.life);
    lifeSprite.setTextureRect(IntRect(0, 700, 90, 90));
    lifeSprite.setScale(0.25f, 0.25f);
    hud.lifeIcons.clear();
    for (int i = 0; i < state.lives; ++i)
    {
        Sprite icon = lifeSprite;
        icon.setPosition(30.f + i * 50.f, 75.f);
        hud.lifeIcons.push_back(icon);
    }
    RectangleShape gamingBackground(Vector2f(1280, 720));
    gamingBackground.setTexture(&res.background);
    gamingBackground.setFillColor(Color(255, 255, 255, 162));
    RectangleShape gamingBackground2(Vector2f(1280, 720));
    gamingBackground2.setTexture(&res.background);
    gamingBackground2.setFillColor(Color(255, 255, 255, 162));
    gamingBackground2.setPosition(Vector2f(1280, 0));
    RectangleShape bgFar1(Vector2f(1280, 720));
    bgFar1.setTexture(&res.background);
    bgFar1.setFillColor(Color(255, 255, 255, 255));
    RectangleShape bgFar2(Vector2f(1280, 720));
    bgFar2.setTexture(&res.background);
    bgFar2.setPosition(Vector2f(1280, 0));
    bgFar2.setFillColor(Color(255, 255, 255, 255));
    RectangleShape pauseOverlay(Vector2f(1280, 720));
    pauseOverlay.setFillColor(Color(0, 0, 0, 160));

    const float progressStart = progressBg.getPosition().x;
    const float progressEnd = progressBg.getPosition().x + progressBg.getSize().x;
    const float progressMarkerY = progressBg.getPosition().y + progressBg.getSize().y / 2.f;
    RectangleShape vignette(Vector2f(1280.f, 720.f));
    vignette.setFillColor(Color(0, 0, 0, 35));

    // Core game state and timing helpers
    Vector2f pos;                                               
    Clock clock, clockAnimPlay, clockAnimMeteor, clockAnimText;   
    View baseView = win.getDefaultView();


    
    Texture texplayer = res.player;
    Sprite player;
    player.setTexture(texplayer);
    player.setTextureRect(IntRect(0, state.playerAnim.Frame, 90, 90));
    player.scale(0.7, 0.7);
    player.setPosition(Vector2f(80, 380));
    // Цель прибытия (планета) показывается после достижения маркера
    Texture textEarth = res.earth;
    RectangleShape Earth(Vector2f(500, 500));
    Earth.setTexture(&textEarth);
    Earth.setPosition(Vector2f(1100, 150));
    
    Sprite destruction;
    destruction.setTexture(res.explosion);
    destruction.setTextureRect(IntRect(5, 15, 95, 80));
    destruction.scale(0.7, 0.7);

    

    Meteor meteor[15];
    const int Nmeteor = 15;

    Fuel canister("Image/galon.png", 1000, 1000);


    while (win.isOpen())  
    {
        // Compute elapsed time once per frame and derive speeds for subsystems
        state.deltaSeconds = clock.restart().asSeconds();
        state.shakeTimer = std::max(0.f, state.shakeTimer - state.deltaSeconds);
        state.flashTimer = std::max(0.f, state.flashTimer - state.deltaSeconds);
        state.smokeAccumulator += state.deltaSeconds;
        state.shieldPulse += state.deltaSeconds;
        if (state.shieldActive)
        {
            state.shieldTimer -= state.deltaSeconds;
            if (state.shieldTimer <= 0.f) state.shieldActive = false;
        }
        baseView = win.getDefaultView();
        bool thrustingFrame = false;
        //----------------------------------------------

        handleEvents(win, state, game_music, mus, Rcircle, progressEnd);
        if (!state.gamePause) {
            // Основной игровой цикл пока ракета не достигла контрольной точки
            if (Rcircle.getPosition().x <= progressEnd) {
                if (state.gameOver)
                {
                    // Проигрышная анимация (взрыв), затем мягкий рестарт
                    if (clockAnimMeteor.getElapsedTime() > milliseconds(80))
                    {
                        clockAnimMeteor.restart();
                        state.destructAnim.Frame += state.destructAnim.Step;
                        if (state.destructAnim.Frame > 405) { state.destructAnim.Frame1 += state.destructAnim.Step1; state.destructAnim.Frame = 5; }
                        if (state.destructAnim.Frame1 > 415) {
                            state.gameOver = false;
                            player.setPosition(Vector2f(80, 380));
                            for (int i = 0; i < Nmeteor; i++) meteor[i].restart();
                            canister.restart();
                            state.pusk = 0;
                            state.destructAnim.Frame = 5; state.destructAnim.Frame1 = 15;
                            state.fuel = 200.f;
                            state.shieldActive = false; state.shieldTimer = 0.f;
                            state.victoryPlayed = false;
                            if (state.lives <= 0)
                            {
                                // Показать финальный экран и ждать нажатие
                                win.clear();
                                win.setView(baseView);
                                win.draw(bgFar2);
                                win.draw(bgFar1);
                                win.draw(gamingBackground2);
                                win.draw(gamingBackground);
                                win.draw(GameInfoPanel);  
                                win.draw(progressBg);
                                win.draw(progressFill);
                                win.draw(progressShine);
                                win.draw(progressLabel);
                                win.draw(fuelBg);
                                win.draw(fuelFill);
                                win.draw(fuelShine);
                                win.draw(fuelLabel);
                                for (int i = 0; i < state.lives && i < static_cast<int>(hud.lifeIcons.size()); ++i) win.draw(hud.lifeIcons[i]);
                                win.draw(RcircleGlow);
                                win.draw(Rcircle);
                                Text finalMsg, hintMsg;
                                finalMsg.setFont(GameFont);
                                finalMsg.setCharacterSize(80);
                                finalMsg.setFillColor(Color::Red);
                                finalMsg.setString(L"GAME OVER");
                                finalMsg.setPosition(380.f, 280.f);
                                hintMsg.setFont(GameFont);
                                hintMsg.setCharacterSize(30);
                                hintMsg.setFillColor(Color::White);
                                hintMsg.setString(L"Press any key to restart");
                                hintMsg.setPosition(400.f, 380.f);
                                win.draw(finalMsg);
                                win.draw(hintMsg);
                                win.display();
                                // Ждём нажатие любой клавиши
                                bool waitKey = true;
                                Event ev;
                                while (waitKey && win.isOpen())
                                {
                                    while (win.pollEvent(ev))
                                    {
                                        if (ev.type == Event::Closed) { win.close(); return 0; }
                                        if (ev.type == Event::KeyPressed) waitKey = false;
                                    }
                                }
                                // Полный рестарт игры
                                state.lives = 5;
                                Rcircle.setPosition(progressStart, progressMarkerY);
                                canister.restart();
                            }
                        }
                        else { destruction.setTextureRect(IntRect(state.destructAnim.Frame, state.destructAnim.Frame1, 95, 80)); }
                    }
                }
                else {
                    // Реагируем на нажатые клавиши в реальном времени
                    state.traffic = 0;
                    state.moveRec = Vector2f(0.f, 0.f);
                    if (state.fuel > 0.f)
                    {
                        if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Numpad5))
                        {
                            state.moveRec.y += cfg.playerSpeed * state.deltaSeconds;
                            state.traffic = 2;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Numpad8))
                        {
                            state.moveRec.y -= cfg.playerSpeed * state.deltaSeconds;
                            state.traffic = 1;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Numpad4))
                        {
                            state.moveRec.x -= cfg.playerSpeed * state.deltaSeconds;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Numpad6))
                        {
                            state.moveRec.x += cfg.playerSpeed * state.deltaSeconds;
                        }
                    }

                    bool thrusting = (state.moveRec.x != 0.f || state.moveRec.y != 0.f) && state.fuel > 0.f;
                    if (thrusting)
                    {
                        thrustingFrame = true;
                        if (!state.shieldActive)
                            state.fuel = std::max(0.f, state.fuel - cfg.fuelDrainPerSecond * state.deltaSeconds);
                        if (state.smokeAccumulator >= 0.04f)
                        {
                            state.smokeAccumulator = 0.f;
                            FloatRect pb = player.getGlobalBounds();
                            SmokeParticle p;
                            // Приблизительно в районе сопла (левее и выше)
                            p.pos = Vector2f(pb.left + pb.width * 0.18f + static_cast<float>(rand() % 4 - 2),
                                             pb.top + pb.height * 0.50f + static_cast<float>(rand() % 4 - 2));
                            p.ttl = 0.45f;
                            p.lifetime = p.ttl;
                            p.size = 6.f + static_cast<float>(rand() % 5);
                            state.smoke.push_back(p);
                        }
                    }

                    // Анимация игрока (работает только при движении)
                    if (clockAnimPlay.getElapsedTime() > milliseconds(100))
                    {
                        clockAnimPlay.restart();
                        playeranim(player, state.playerAnim, state.traffic);

                    }

                    // Движение астероидов и индикатора полёта
                    if (clockAnimMeteor.getElapsedTime() > milliseconds(80))
                    {
                        clockAnimMeteor.restart();
                        for (int i = 0; i < Nmeteor; i++) meteor[i].animation();
                    }

                    // Плавное исчезновение подсказки о подобранном топливе
                    if (clockAnimText.getElapsedTime() > milliseconds(50))
                    {
                        clockAnimText.restart();
                        if (state.pusk > 0)
                        {
                            state.pusk--;
                            fuelGainText.move(0, -1);
                        }
                    }

                    // Бесшовное перемещение фоновых картинок (два слоя для параллакса)
                    bgFar1.move(-cfg.backgroundSpeedFar * state.deltaSeconds, 0);
                    pos = bgFar1.getPosition();
                    if (pos.x < -1280) bgFar1.setPosition(1280, pos.y);
                    bgFar2.move(-cfg.backgroundSpeedFar * state.deltaSeconds, 0);
                    pos = bgFar2.getPosition();
                    if (pos.x < -1280) bgFar2.setPosition(1280, pos.y);

                    gamingBackground.move(-cfg.backgroundSpeed * state.deltaSeconds, 0);
                    pos = gamingBackground.getPosition();
                    if (pos.x < -1280) gamingBackground.setPosition(1280, pos.y);
                    gamingBackground2.move(-cfg.backgroundSpeed * state.deltaSeconds, 0);
                    pos = gamingBackground2.getPosition();
                    if (pos.x < -1280) gamingBackground2.setPosition(1280, pos.y);
                    Rcircle.move(cfg.progressSpeed * state.deltaSeconds, 0);
                    //-----------------------------------------------------------------------

                    PlayerMove(player, state.moveRec); // применяем накопленное движение от клавиш
                    FloatRect playerHitbox = ShrinkRect(player.getGlobalBounds(), 0.18f);

                    // Обновляем дымовые частицы
                    for (auto it = state.smoke.begin(); it != state.smoke.end(); )
                    {
                        it->lifetime -= state.deltaSeconds;
                        it->pos.x -= cfg.backgroundSpeed * state.deltaSeconds * 0.5f;
                        it->pos.y -= 10.f * state.deltaSeconds;
                        if (it->lifetime <= 0.f) it = state.smoke.erase(it);
                        else ++it;
                    }
                    // Локальные вспышки после подбора топлива
                    for (auto it = state.flashes.begin(); it != state.flashes.end(); )
                    {
                        it->lifetime -= state.deltaSeconds;
                        if (it->lifetime <= 0.f) it = state.flashes.erase(it);
                        else ++it;
                    }

                    for (int i = 0; i < Nmeteor; i++) {
                        if (meteor[i].newborn) Correct(canister, i, meteor, Nmeteor);
                        meteor[i].move(state.deltaSeconds);          // 
                        FloatRect meteorHitbox = ShrinkRect(meteor[i].getMeteorBounds(), 0.12f);
                        if (meteorHitbox.intersects(playerHitbox))
                        {
                            if (state.shieldActive)
                            {
                                Vector2f hitPos = meteor[i].getPosBonus();
                                meteor[i].restart();
                                state.flashes.push_back(PickupFlash{ hitPos, 0.25f, 0.25f });
                            }
                            else
                            {
                                state.lives = std::max(0, state.lives - 1);
                                falsound.play(); state.gameOver = true; destruction.setPosition(player.getPosition().x, player.getPosition().y); state.shakeTimer = 0.35f; state.flashTimer = 0.25f; break;
                            }
                        }
                    }
                    // 
                    if (canister.newborn) CorrectFuel(canister, meteor, Nmeteor);
                    canister.move(state.deltaSeconds); //
                    // Подобранное топливо/щит
                    if (ShrinkRect(canister.getMeteorBounds(), 0.12f).intersects(playerHitbox))
                    {
                        if (canister.getType() == BonusType::Shield)
                        {
                            state.shieldActive = true;
                            state.shieldTimer = cfg.shieldDuration;
                            soundbonB.play();
                            fuelGainText.setString(L"SHIELD");
                            fuelGainText.setFillColor(Color::Red);
                            fuelGainText.setPosition(canister.getPosBonus().x, canister.getPosBonus().y);
                            state.pusk = 30;
                            state.flashes.push_back(PickupFlash{ player.getPosition(), 0.25f, 0.25f });
                        }
                        else
                        {
                            fuelGainText.setFillColor(Color::Green);
                            state.fuelGain = canister.getAmount();
                            state.fuel = std::min(cfg.maxFuel, state.fuel + static_cast<float>(state.fuelGain));
                            if (state.fuelGain > 50) soundbonB.play(); else soundbonM.play();
                            fuelGainText.setString(IntToStr(static_cast<int>(state.fuelGain)));
                            fuelGainText.setPosition(fuelBg.getPosition().x + fuelBg.getSize().x - 30.f, fuelBg.getPosition().y + 22.f);
                            state.pusk = 40;
                        }
                        canister.restart();
                        state.flashes.push_back(PickupFlash{ player.getPosition(), 0.25f, 0.25f });
                    }
                    // Полосы прогресса/топлива
                    float fuelPercent = std::clamp(state.fuel / cfg.maxFuel, 0.f, 1.f);
                    float fuelWidth = fuelBg.getSize().x * fuelPercent;
                    fuelFill.setSize(Vector2f(fuelWidth, fuelBg.getSize().y));
                    fuelShine.setSize(Vector2f(fuelWidth, fuelShine.getSize().y));
                    if (fuelPercent >= 0.5f) fuelFill.setFillColor(Color::Green);
                    else if (fuelPercent >= 0.25f) fuelFill.setFillColor(Color::Yellow);
                    else fuelFill.setFillColor(Color::Red);
                    float progressPercent = std::clamp((Rcircle.getPosition().x - progressStart) / (progressEnd - progressStart), 0.f, 1.f);
                    float progressWidth = progressBg.getSize().x * progressPercent;
                    progressFill.setSize(Vector2f(progressWidth, progressBg.getSize().y));
                    progressShine.setSize(Vector2f(progressWidth, progressShine.getSize().y));
                }
                
                win.clear();               
                View view = baseView;
                if (state.shakeTimer > 0.f)
                {
                    float strength = 6.f * (state.shakeTimer / 0.35f);
                    float ox = (static_cast<float>(rand() % 200) / 100.f - 1.f) * strength;
                    float oy = (static_cast<float>(rand() % 200) / 100.f - 1.f) * strength;
                    view.move(ox, oy);
                }
                win.setView(view);

                win.draw(bgFar2);
                win.draw(bgFar1);
                win.draw(gamingBackground2);
                win.draw(gamingBackground);
                win.draw(vignette);
                // HUD + основные объекты
                drawHUD(state, win, GameInfoPanel, progressBg, progressFill, progressShine, progressLabel,
                        fuelBg, fuelFill, fuelShine, fuelLabel, fuelMax, hud.lifeIcons, Rcircle, RcircleGlow);
                // Дым рисуем под кораблём
                for (const auto& s : state.smoke)
                {
                    CircleShape puff(s.size);
                    float alpha = static_cast<float>(std::max(0.f, s.lifetime)) / s.ttl;
                    float faded = std::pow(alpha, 0.8f);
                    puff.setFillColor(Color(
                        230,
                        230,
                        230,
                        static_cast<Uint8>(170 * faded)));
                    puff.setPosition(s.pos);
                    win.draw(puff);
                }
                // Вспышки от подбора топлива
                for (const auto& f : state.flashes)
                {
                    float k = f.lifetime / f.ttl;
                    float radius = 10.f + 25.f * (1.f - k);
                    CircleShape flash(radius);
                    flash.setOrigin(radius, radius);
                    flash.setPosition(f.pos);
                    flash.setFillColor(Color(255, 200, 80, static_cast<Uint8>(190 * k)));
                    win.draw(flash);
                    CircleShape flashCore(radius * 0.5f);
                    flashCore.setOrigin(radius * 0.5f, radius * 0.5f);
                    flashCore.setPosition(f.pos);
                    flashCore.setFillColor(Color(255, 255, 255, static_cast<Uint8>(200 * k)));
                    win.draw(flashCore);
                }

                if (thrustingFrame)
                {
                    FloatRect pb = player.getGlobalBounds();
                    CircleShape glow(18.f);
                    glow.setFillColor(Color(255, 200, 80, 160));
                    glow.setPosition(pb.left + pb.width * 0.1f, pb.top + pb.height * 0.42f);
                    win.draw(glow);
                }

                if (state.gameOver) {
                    win.draw(destruction);
                } else {
                    // Контровка по форме спрайта: чуть увеличенная прозрачная копия
                    const float outlineScale = 1.2f;
                    auto lb = player.getLocalBounds();
                    Vector2f center(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
                    Vector2f worldCenter(player.getPosition().x + player.getGlobalBounds().width / 2.f,
                                         player.getPosition().y + player.getGlobalBounds().height / 2.f);
                    Sprite outline = player;
                    outline.setOrigin(center);
                    outline.setPosition(worldCenter);
                    outline.setScale(player.getScale().x * outlineScale, player.getScale().y * outlineScale);
                    outline.setColor(Color(100, 180, 255, 190));
                    RenderStates addBlend;
                    addBlend.blendMode = BlendAdd;
                    if (state.shieldActive)
                    {
                        // Красное мерцание при активном щите
                        float pulse = 0.5f + 0.5f * std::sin(state.shieldPulse * 10.f);
                        outline.setColor(Color(255, 60, 60, static_cast<Uint8>(140 + 100 * pulse)));
                        win.draw(outline, addBlend);
                    }
                    else
                    {
                        win.draw(outline, addBlend);
                    }
                    if (state.shieldActive)
                    {
                        float ringRadius = std::max(player.getGlobalBounds().width, player.getGlobalBounds().height) * 0.8f;
                        CircleShape ring(ringRadius);
                        ring.setOrigin(ringRadius, ringRadius);
                        ring.setPosition(player.getPosition().x + player.getGlobalBounds().width / 2.f,
                                         player.getPosition().y + player.getGlobalBounds().height / 2.f);
                        float pulse = 0.5f + 0.5f * std::sin(state.shieldPulse * 12.f);
                        ring.setOutlineThickness(3.f);
                        ring.setOutlineColor(Color(255, 80, 80, static_cast<Uint8>(120 + 80 * pulse)));
                        ring.setFillColor(Color(255, 50, 50, static_cast<Uint8>(40 * pulse)));
                        win.draw(ring, addBlend);
                    }
                    win.draw(player);
                }     
                for (int i = 0; i < Nmeteor; i++)  meteor[i].draw(win);          
                canister.draw(win);
                if (state.pusk > 0) win.draw(fuelGainText);
                if (state.flashTimer > 0.f)
                {
                    RectangleShape flash(Vector2f(1280.f, 720.f));
                    flash.setFillColor(Color(255, 255, 255, static_cast<Uint8>(255 * (state.flashTimer / 0.25f))));
                    win.draw(flash);
                }
                win.display();  
                if (Rcircle.getPosition().x >= progressEnd && !state.victoryPlayed) {
                    Rcircle.setPosition(progressEnd, Rcircle.getPosition().y);
                    game_music.stop();
                    soundvic.play();
                    state.victoryPlayed = true;
                }
                //------------------------------------------------------------------------
            } 
            else 
            {
                // Финиш: показываем планету и надпись победы, закрываем через таймаут
                win.clear();               
                
                win.setView(baseView);
                win.draw(bgFar2);
                win.draw(bgFar1);
                win.draw(gamingBackground2);
                win.draw(gamingBackground);
                win.draw(vignette);
                drawHUD(state, win, GameInfoPanel, progressBg, progressFill, progressShine, progressLabel,
                        fuelBg, fuelFill, fuelShine, fuelLabel, fuelMax, hud.lifeIcons, Rcircle, RcircleGlow);
                win.draw(Earth);
                win.draw(win_title);
                win.draw(win_sub);
                win.display();            
                if (clockAnimText.getElapsedTime() > seconds(20)) win.close();
            }
        }//Pause
        else  //Pause
        {
            // Простой экран паузы без остановки музыки
            win.clear();
            win.setView(baseView);
            win.draw(bgFar2);
            win.draw(bgFar1);
            win.draw(gamingBackground2);
            win.draw(gamingBackground);
            win.draw(GameInfoPanel);
            win.draw(progressBg);
            win.draw(progressFill);
            win.draw(progressShine);
            win.draw(progressLabel);
            win.draw(fuelBg);
            win.draw(fuelFill);
            win.draw(fuelShine);
            win.draw(fuelLabel);
            drawHUD(state, win, GameInfoPanel, progressBg, progressFill, progressShine, progressLabel,
                    fuelBg, fuelFill, fuelShine, fuelLabel, fuelMax, hud.lifeIcons, Rcircle, RcircleGlow);
            win.draw(pauseOverlay);
            win.draw(text_pause);
            win.display();
        }

    }

    return 0;
}

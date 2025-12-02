#include "worklibgamer.h"

void PlayerMove(sf::Sprite& player, sf::Vector2f moveRec)
{
    // Move with current velocity then clamp to on-screen bounds
    player.move(moveRec);
    sf::Vector2f pos = player.getPosition();
    if (pos.x > 1200) player.setPosition(1200, pos.y);
    if (pos.x < 50) player.setPosition(50, pos.y);
    if (pos.y > 660) player.setPosition(pos.x, 660);
    if (pos.y < 110) player.setPosition(pos.x, 110);

    if ((pos.y > 1200) && (pos.y < 110)) player.setPosition(1200, 110);
    if ((pos.y > 1200) && (pos.y > 660)) player.setPosition(1200, 660);
    if ((pos.y < 50) && (pos.y < 110)) player.setPosition(50, 110);
    if ((pos.y < 50) && (pos.y > 660)) player.setPosition(50, 660);
}

void playeranim(sf::Sprite& player, game::FrameAnim& FramePlAnim, int traffic)
{
    // Cycle through sprite sheet rows depending on whether ship is moving
    FramePlAnim.Frame += FramePlAnim.Step;
    player.setTextureRect(sf::IntRect(0, FramePlAnim.Frame, 90, 90));
    if (traffic) if (FramePlAnim.Frame > 0) FramePlAnim.Step = -100;
    else FramePlAnim.Step = 0;
    else
    {
        if (FramePlAnim.Frame == 800) FramePlAnim.Step = 0;
        if (FramePlAnim.Frame <= 700) FramePlAnim.Step = 100;
    }
}

void Correct(Fuel& canister, int i, Meteor* meteor, int nmeteor)
{
    // If a meteor overlaps another object right after spawn, respawn it recursively
    for (int i1 = 0; i1 < nmeteor; i1++)
    {
        if (i1 != i) {
            if (meteor[i1].collision(meteor[i].getMeteorBounds()))
            {
                meteor[i].restart();
                Correct(canister, i, meteor, nmeteor);
                break;
            }
        }
    }
    if (canister.collision(meteor[i].getMeteorBounds())) 
    {
        meteor[i].restart();
        Correct(canister, i, meteor, nmeteor);
    }
}

void CorrectFuel(Fuel& canister, Meteor* meteor, int nmeteor)
{
    // Keep fuel from spawning directly on top of meteors
    for (int i1 = 0; i1 < nmeteor; i1++)
    {
        if (meteor[i1].collision(canister.getMeteorBounds()))
            {
                canister.restart();
                CorrectFuel(canister, meteor, nmeteor);
                break;
            }
        }
    }

sf::FloatRect ShrinkRect(const sf::FloatRect& rect, float ratio)
{
    // Reduce bounds uniformly on each side by given fraction (0..0.5)
    float insetX = rect.width * ratio;
    float insetY = rect.height * ratio;
    return sf::FloatRect(rect.left + insetX, rect.top + insetY, rect.width - 2 * insetX, rect.height - 2 * insetY);
}

std::string IntToStr(int number)
{
    std::ostringstream TextString;
    TextString << number;
    return TextString.str();
}

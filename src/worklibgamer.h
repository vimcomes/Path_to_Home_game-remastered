#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Meteor.h"
#include "Fuel.h"
#include "GameState.h"
#include <sstream>
#include <string>

// Moves player sprite and clamps it to the visible play area
void PlayerMove(sf::Sprite& player, sf::Vector2f moveRec);

// Updates player texture rect to reflect current animation frame
void playeranim(sf::Sprite& player, game::FrameAnim& FramePlAnim, int traffic);

// Ensures newly spawned meteor does not overlap other meteors or the fuel canister
void Correct(Fuel& canister, int i, Meteor* meteor, int nmeteor);

// Ensures newly spawned fuel does not overlap any active meteor
void CorrectFuel(Fuel& canister, Meteor* meteor, int nmeteor);

// Shrinks rectangle bounds by ratio from each side (for tighter hitboxes)
sf::FloatRect ShrinkRect(const sf::FloatRect& rect, float ratio);

std::string IntToStr(int number);

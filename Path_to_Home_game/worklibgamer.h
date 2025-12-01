#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <string>
#include "Meteor.h"
#include "Fuel.h"

using sf::FloatRect;
using sf::Sprite;
using sf::Vector2f;
using std::string;

struct FrameAnim
{
	int Frame = 0;
	int Step = 100;
	int Frame1 = 0;
	int Step1 = 100;
};

// Moves player sprite and clamps it to the visible play area
void PlayerMove(Sprite & player, Vector2f moveRec);

// Updates player texture rect to reflect current animation frame
void playeranim(Sprite& player, FrameAnim& FramePlAnim, int traffic);

// Ensures newly spawned meteor does not overlap other meteors or the fuel canister
void Correct(Fuel& canister, int i, Meteor * meteor, int nmeteor);

// Ensures newly spawned fuel does not overlap any active meteor
void CorrectFuel(Fuel& canister, Meteor* meteor, int nmeteor);

// Shrinks rectangle bounds by ratio from each side (for tighter hitboxes)
FloatRect ShrinkRect(const FloatRect& rect, float ratio);

string IntToStr(int number);

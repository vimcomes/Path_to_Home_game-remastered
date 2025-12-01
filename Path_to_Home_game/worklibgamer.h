#pragma once
#include <SFML/Graphics.hpp>
#include "Meteor.h"
#include "Fuel.h"
#include <sstream>
#include <string>
#include <SFML/Audio.hpp>

using namespace std;

using namespace sf;

struct FrameAnim
{
	int Frame = 0;
	int Step = 100;
	int Frame1 = 0;
	int Step1 = 100;
};

/** @brief SpaceShip Moving
* 
*
*/
void PlayerMove(Sprite & player, Vector2f moveRec);

/**
* @ brief Animation
*/
void playeranim(Sprite& player, FrameAnim& FramePlAnim, int traffic);

void Correct(Fuel& canister, int i, Meteor * meteor, int nmeteor);

void CorrectFuel(Fuel& canister, Meteor* meteor, int nmeteor);

string IntToStr(int number);
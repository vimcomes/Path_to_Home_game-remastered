#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Meteor
{
public:
	Meteor(); // constructor
	~Meteor(); // destructor;

	void move(float& time); // meteor move
	void draw(RenderWindow& window); // meteor draw
	void animation(); // meteor animation
	bool collision(FloatRect object); //meteor collision
	virtual void restart(); // set new coords
	const FloatRect getMeteorBounds(); // position in global coords
	bool newborn;

	Vector2f getPosBonus()
	{
		return PosBonus;
	}

protected:
	Sprite SpaceObject; // meteor object
	Texture TextureObject; // meteor texure
	Vector2f PosBonus;
private:



	int xsp[5]{ 3,73,135,198,262 }; //coords by x
	int ysp[6]{ 3,68,132,200,265,325 }; // coords by y
	int ix, iy = 0; // offsets by x, y
	int st = 0; // animation step
};


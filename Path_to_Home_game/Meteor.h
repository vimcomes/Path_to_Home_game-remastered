#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Meteor
{
public:
	Meteor(); // constructor
	~Meteor(); // destructor;

	void move(float delta); // move meteor across the screen with time delta
	void draw(RenderWindow& window); // render meteor sprite
	void animation(); // animate meteor texture if enabled
	bool collision(FloatRect object); // collision check with arbitrary rect
	virtual void restart(); // respawn off-screen with random size/position
	const FloatRect getMeteorBounds(); // bounds in global coords
	bool newborn; // true when the meteor was just respawned

	Vector2f getPosBonus()
	{
		return PosBonus;
	}

protected:
	Sprite SpaceObject; // meteor object
	Texture TextureObject; // meteor texure
	Vector2f PosBonus;
	float spinSpeed = 0.f; // deg/sec
private:



	int xsp[5]{ 3,73,135,198,262 }; //coords by x
	int ysp[6]{ 3,68,132,200,265,325 }; // coords by y
	int ix, iy = 0; // offsets by x, y
	int st = 0; // animation step
};

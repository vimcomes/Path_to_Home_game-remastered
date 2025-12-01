#include "Meteor.h"


Meteor::Meteor()
{
	TextureObject.loadFromFile("Image/asteroid.png");
	SpaceObject.setTexture(TextureObject);
	SpaceObject.setTextureRect(IntRect(262, 325, 55, 50));
	restart(); // pick initial random spawn
}

Meteor::~Meteor()
{

}

void Meteor::move(float delta)
{
	// drift left; respawn when off-screen to recycle objects
	newborn = false;
	const float meteorSpeed = 160.f;
	SpaceObject.move(-meteorSpeed * delta,0);
	if (spinSpeed != 0.f) SpaceObject.rotate(spinSpeed * delta);
	PosBonus = SpaceObject.getPosition();
	if (SpaceObject.getPosition().x < -60) restart();
}

void Meteor::draw(RenderWindow& window) 
{
	window.draw(SpaceObject);
}

void Meteor::animation()
{
	// step through sprite sheet grid when animation is enabled
	if (st > 0)
	{
		SpaceObject.setTextureRect(IntRect(xsp[ix], ysp[iy], 50,45));
		ix += st;
		if (ix > 4) 
		{
			ix = 0; iy++;
			if (iy > 5) iy = 0;
		}
	}
}

bool Meteor::collision(FloatRect object)
{
	if (SpaceObject.getGlobalBounds().intersects(object)) return true;
	return false;
}

void Meteor::restart()
{
	// Respawn meteor to the right side with random scale and animation offsets
	newborn = true;
	// Смещаем распределение в сторону мелких объектов для разнообразия
	int roll = rand() % 100;
	float s = 0.f;
	if (roll < 55)      s = static_cast<float>(rand() % 12 + 6);   // мелкие
	else if (roll < 85) s = static_cast<float>(rand() % 12 + 14);  // средние
	else                s = static_cast<float>(rand() % 10 + 24);  // крупные
	float x = static_cast<float>(rand() % 1280 + 1280);
	float y = static_cast<float>(rand() % 540 + 130);

	SpaceObject.setPosition(Vector2f(x, y));
	SpaceObject.setScale(s/20, s/20);
	ix = rand() % 4;
	iy = rand() % 5;
	st = 1 + rand() % 2;
	spinSpeed = static_cast<float>((rand() % 121) - 60); // -60..60 deg/sec
	PosBonus = SpaceObject.getPosition();
}

const FloatRect Meteor::getMeteorBounds()
{
	return SpaceObject.getGlobalBounds();
}

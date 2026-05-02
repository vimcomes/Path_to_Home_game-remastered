#include "Meteor.h"
#include "GameState.h"
#include "ResourcePaths.h"
#include <stdexcept>


Meteor::Meteor()
{
	if (!TextureObject.loadFromFile(resourcePath("images/asteroid.png")))
		throw std::runtime_error("Failed to load asteroid texture");
	SpaceObject.setTexture(TextureObject);
	SpaceObject.setTextureRect(sf::IntRect(262, 325, 55, 50));
	restart();
}

Meteor::~Meteor()
{

}

void Meteor::move(float delta)
{
	newborn = false;
	SpaceObject.move(-game::constants::MeteorSpeed * delta, 0.f);
	if (spinSpeed != 0.f) SpaceObject.rotate(spinSpeed * delta);
	PosBonus = SpaceObject.getPosition();
	if (SpaceObject.getPosition().x < -60) restart();
}

void Meteor::draw(sf::RenderWindow& window) 
{
	window.draw(SpaceObject);
}

void Meteor::animation()
{
	// step through sprite sheet grid when animation is enabled
	if (st > 0)
	{
		SpaceObject.setTextureRect(sf::IntRect(xsp[ix], ysp[iy], 50,45));
		ix += st;
		if (ix > 4) 
		{
			ix = 0; iy++;
			if (iy > 5) iy = 0;
		}
	}
}

bool Meteor::collision(sf::FloatRect object)
{
	if (SpaceObject.getGlobalBounds().intersects(object)) return true;
	return false;
}

void Meteor::restart()
{
	newborn = true;
	int roll = game::randomInt(0, 99);
	float s = 0.f;
	if (roll < 55)      s = static_cast<float>(game::randomInt(6, 17));
	else if (roll < 85) s = static_cast<float>(game::randomInt(14, 25));
	else                s = static_cast<float>(game::randomInt(24, 33));
	float x = static_cast<float>(game::randomInt(1280, 2559));
	float y = static_cast<float>(game::randomInt(130, 669));

	SpaceObject.setPosition(sf::Vector2f(x, y));
	SpaceObject.setScale(s/20, s/20);
	ix = game::randomInt(0, 3);
	iy = game::randomInt(0, 4);
	st = 1 + game::randomInt(0, 1);
	spinSpeed = game::randomFloat(-60.f, 60.f);
	PosBonus = SpaceObject.getPosition();
}

const sf::FloatRect Meteor::getMeteorBounds()
{
	return SpaceObject.getGlobalBounds();
}

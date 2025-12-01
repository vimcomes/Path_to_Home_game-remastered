#include "Fuel.h"
#include <cstdlib>

Fuel::Fuel(const sf::String& name, int x, int y)
{
	// Initialize texture and scale so the canister roughly matches meteor size
	TextureObject.loadFromFile(name);
	SpaceObject.setTexture(TextureObject);
	SpaceObject.setTextureRect(sf::IntRect(0, 0, x, y));
	int scal = 1000 / x;
	baseScale = 0.05f * scal;
	SpaceObject.setScale(baseScale, baseScale);
	restart();
}

Fuel::~Fuel()
{
}

void Fuel::restart()
{
	// Spawn fuel off-screen to the right at a random height
	newborn = true;
	type = (rand() % 100 < 35) ? BonusType::Shield : BonusType::Fuel; // 35% шанс щита
	amount = (type == BonusType::Fuel) ? 10 + rand() % 81 : 0;
	float x = static_cast<float>(rand() % 1280 + 1280);
	float y = static_cast<float>(rand() % 540 + 130);

	SpaceObject.setPosition(sf::Vector2f(x, y));
	PosBonus = SpaceObject.getPosition();
	updateScaleFromAmount();
	if (type == BonusType::Shield)
	{
		SpaceObject.setColor(sf::Color(120, 240, 255, 240));
		spinSpeed = static_cast<float>((rand() % 101) - 50);
	}
	else
	{
		SpaceObject.setColor(sf::Color::White);
		spinSpeed = static_cast<float>((rand() % 121) - 60);
	}
}

void Fuel::updateScaleFromAmount()
{
	// Scale the sprite based on fuel amount: small cans stay small, big ones noticeable
	if (type == BonusType::Shield)
	{
		// Щит — заметный пульсирующий шар
		const float shieldScale = 1.1f;
		SpaceObject.setScale(baseScale * shieldScale, baseScale * shieldScale);
		return;
	}
	const float minScale = 0.7f;
	const float maxScale = 1.35f;
	float t = static_cast<float>(amount - 10) / 80.f; // 0..1
	float factor = minScale + (maxScale - minScale) * t;
	SpaceObject.setScale(baseScale * factor, baseScale * factor);
}

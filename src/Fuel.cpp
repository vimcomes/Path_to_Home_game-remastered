#include "Fuel.h"
#include "GameState.h"
#include <stdexcept>

Fuel::Fuel(const sf::String& name, int x, int y)
{
	if (!TextureObject.loadFromFile(name))
		throw std::runtime_error("Failed to load fuel texture");
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
	newborn = true;
	type = (game::randomInt(0, 99) < 35) ? BonusType::Shield : BonusType::Fuel;
	amount = (type == BonusType::Fuel) ? 10 + game::randomInt(0, 80) : 0;
	float x = static_cast<float>(game::randomInt(1280, 2559));
	float y = static_cast<float>(game::randomInt(130, 669));

	SpaceObject.setPosition(sf::Vector2f(x, y));
	PosBonus = SpaceObject.getPosition();
	updateScaleFromAmount();
	if (type == BonusType::Shield)
	{
		SpaceObject.setColor(sf::Color(120, 240, 255, 240));
		spinSpeed = game::randomFloat(-50.f, 50.f);
	}
	else
	{
		SpaceObject.setColor(sf::Color::White);
		spinSpeed = game::randomFloat(-60.f, 60.f);
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

#include "LavaProjectile.h"
#include "../Monster/Monster.h"
#include <cmath>

LavaProjectile::LavaProjectile(sf::Vector2f startPos, sf::Vector2f targetPos, int damage, float speed)
	: targetPos_(targetPos), speed_(speed), landed_(false), floorDamage_(damage)
{
	isActive_ = true;
	position_ = startPos;
	setPosition(startPos);
	setSize({20, 20});
	setTextureId("lava_bucket");
	lifetime_ = 5.0f;
}

void LavaProjectile::update(float deltaTime)
{
	if (!isActive_)
		return;

	sf::Vector2f dir = targetPos_ - position_;
	float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if (dist < 10.0f)
	{
		landed_ = true;
		isActive_ = false;
		pendingFloors.push_back({position_, 5.0f, floorDamage_, 0.0f});
		return;
	}
	dir /= dist;
	position_ += dir * speed_ * deltaTime;
	shape_.setPosition(position_);

	lifetime_ -= deltaTime;
	if (lifetime_ <= 0.0f)
	{
		landed_ = true;
		isActive_ = false;
		pendingFloors.push_back({position_, 5.0f, floorDamage_, 0.0f});
	}
}

void LavaProjectile::onHit(Monster &monster)
{
	(void)monster;
}

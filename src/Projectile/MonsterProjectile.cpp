#include "MonsterProjectile.h"
#include "../Monster/Monster.h"
#include <cmath>

MonsterProjectile::MonsterProjectile()
	: speed_(300.0f), areaDamage_(0), areaRadius_(0.0f), areaDuration_(0.0f),
	  areaBuffType_(BuffType::Wither)
{
	setSize({24, 24});
	std::random_device rd;
	rng_ = std::mt19937(rd());
}

bool MonsterProjectile::shouldSpawnArea()
{
	if (areaDuration_ <= 0.0f || !onSpawnArea_)
		return false;
	if (areaChance_ >= 1.0f)
		return true;
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	return dist(rng_) < areaChance_;
}

void MonsterProjectile::update(float deltaTime)
{
	if (!isActive_)
		return;

	position_ += direction_ * speed_ * deltaTime;
	shape_.setPosition(position_);

	lifetime_ -= deltaTime;
	if (lifetime_ <= 0.0f)
	{
		if (shouldSpawnArea())
			onSpawnArea_(position_, areaRadius_, areaDuration_, areaBuffType_, areaDamage_);
		isActive_ = false;
	}
}

void MonsterProjectile::onHit(Monster &monster)
{
	monster.takeDamage(damage_);
	if (shouldSpawnArea())
		onSpawnArea_(position_, areaRadius_, areaDuration_, areaBuffType_, areaDamage_);
	isActive_ = false;
}

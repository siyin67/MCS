#include "MonsterProjectile.h"
#include "../Monster/Monster.h"
#include <cmath>

MonsterProjectile::MonsterProjectile()
	: speed_(300.0f), areaDamage_(0), areaRadius_(0.0f), areaDuration_(0.0f),
	  areaBuffType_(BuffType::Wither)
{
	setSize({24, 24});
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
		if (areaDuration_ > 0.0f && onSpawnArea_)
			onSpawnArea_(position_, areaRadius_, areaDuration_, areaBuffType_, areaDamage_);
		isActive_ = false;
	}
}

void MonsterProjectile::onHit(Monster &monster)
{
	monster.takeDamage(damage_);
	if (areaDuration_ > 0.0f && onSpawnArea_)
		onSpawnArea_(position_, areaRadius_, areaDuration_, areaBuffType_, areaDamage_);
	isActive_ = false;
}

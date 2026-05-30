#pragma once
#include "Projectile.h"
#include <functional>
#include <random>

class MonsterProjectile : public Projectile
{
private:
	sf::Vector2f direction_;
	float speed_;
	int areaDamage_;
	float areaRadius_;
	float areaDuration_;
	float areaChance_ = 1.0f;
	BuffType areaBuffType_;
	std::function<void(sf::Vector2f, float, float, BuffType, int)> onSpawnArea_;
	std::mt19937 rng_;

	bool shouldSpawnArea();

public:
	MonsterProjectile();

	void setDirection(const sf::Vector2f &dir) { direction_ = dir; }
	void setSpeed(float speed) { speed_ = speed; }
	void setAreaParams(BuffType type, float radius, float duration, int damage)
	{
		areaBuffType_ = type;
		areaRadius_ = radius;
		areaDuration_ = duration;
		areaDamage_ = damage;
	}
	void setAreaChance(float chance) { areaChance_ = chance; }
	BuffType getAreaBuffType() const { return areaBuffType_; }
	float getAreaRadius() const { return areaRadius_; }
	float getAreaDuration() const { return areaDuration_; }
	int getAreaDamage() const { return areaDamage_; }
	void setOnSpawnArea(std::function<void(sf::Vector2f, float, float, BuffType, int)> cb) { onSpawnArea_ = std::move(cb); }

	void update(float deltaTime) override;
	void onHit(Monster &monster) override;
};

#pragma once
#include "Monster.h"

class Enderdragon : public Monster
{
private:
	enum class AttackPattern { BurstFan, Circle, RapidSpray };

	float laserTimer_;
	float laserChargeTimer_;
	bool laserCharging_;
	bool laserFiring_;
	sf::Vector2f laserDir_;
	float laserLineLength_;

	float laserCooldown_ = 30.0f;
	float laserChargeTime_ = 3.0f;
	float laserFireTime_ = 1.0f;
	float projSpeed_ = 250.0f;
	float projLifetime_ = 3.0f;
	int projSize_ = 28;
	float areaRadius_ = 100.0f;
	float areaChance_ = 1.0f;
	float areaDuration_ = 5.0f;
	int areaDamage_ = 4;
	float burstDuration_ = 10.0f, circleDuration_ = 6.0f, sprayDuration_ = 4.0f;
	float burstInterval_ = 1.0f, circleInterval_ = 0.6f, sprayInterval_ = 0.12f;
	int burstCount_ = 5, circleCount_ = 16, sprayCount_ = 9;
	float burstAngle_ = 0.15f;
	AttackPattern pattern_;
	float patternTimer_;
	float fireInterval_;

	void fireFromAngle(const sf::Vector2f &dir);

public:
	Enderdragon();
	void playHitSound() override;
	void playDeathSound() override;
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
	void render(sf::RenderWindow &window) override;
};

#pragma once
#include "Monster.h"

class Wither : public Monster
{
private:
	enum class AttackPattern { BurstFan, Circle, RapidSpray };

	float projSpeed_ = 200.0f;
	float projLifetime_ = 3.0f;
	int projSize_ = 24;
	float areaRadius_ = 100.0f;
	float areaChance_ = 1.0f;
	float areaDuration_ = 4.0f;
	int areaDamage_ = 3;
	float burstDuration_ = 10.0f, circleDuration_ = 6.0f, sprayDuration_ = 4.0f;
	float burstInterval_ = 1.0f, circleInterval_ = 0.6f, sprayInterval_ = 0.16f;
	int burstCount_ = 3, circleCount_ = 12, sprayCount_ = 7;
	float burstAngle_ = 0.2f;
	AttackPattern pattern_;
	float patternTimer_;
	float fireInterval_;

	void fireFromAngle(const sf::Vector2f &dir);

public:
	Wither();
	void playHitSound() override;
	void playDeathSound() override;
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
};

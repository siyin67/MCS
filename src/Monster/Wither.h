#pragma once
#include "Monster.h"

class Wither : public Monster
{
private:
	enum class AttackPattern { BurstFan, Circle, RapidSpray };

	AttackPattern pattern_;
	float patternTimer_;
	float fireInterval_;

	void fireFromAngle(const sf::Vector2f &dir);
	static constexpr float BURST_DURATION = 10.0f;
	static constexpr float CIRCLE_DURATION = 6.0f;
	static constexpr float SPRAY_DURATION = 4.0f;

public:
	Wither();
	void playHitSound() override;
	void playDeathSound() override;
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
};

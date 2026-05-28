#pragma once
#include "Monster.h"

class Breeze : public Monster
{
public:
	Breeze();
	void playHitSound() override;
	void playDeathSound() override;
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
};

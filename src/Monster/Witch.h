#pragma once
#include "Monster.h"

class Witch : public Monster
{
private:
	float healRange_ = 250.0f;
	int healAmount_ = 5;

public:
	Witch();
	void playHitSound() override;
	void playDeathSound() override;
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
	void render(sf::RenderWindow &window) override;
};

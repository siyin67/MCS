#pragma once
#include "Monster.h"

class Creeper : public Monster
{
private:
	bool flashing_;
	float flashTimer_;
	float fuseTime_;

public:
	Creeper();
	void playHitSound() override;
	void playDeathSound() override;
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
	void render(sf::RenderWindow &window) override;
};

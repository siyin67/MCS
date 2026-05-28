#pragma once
#include "Monster.h"

class Enderman : public Monster
{
private:
	float teleportCooldown_;

public:
	Enderman();
	void playHitSound() override;
	void playDeathSound() override;
	bool isProjectileImmune() const override { return true; }
	void update(float deltaTime, const sf::Vector2f &playerPos,
				const std::vector<std::unique_ptr<Monster>> &allMonsters) override;
};

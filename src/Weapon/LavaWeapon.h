#pragma once
#include "weapon.h"

class LavaWeapon : public Weapon
{
private:
	float projectileSpeed_;
	int floorDamage_;
	float floorDuration_;

public:
	LavaWeapon();

	void update(float deltaTime) override;
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
};

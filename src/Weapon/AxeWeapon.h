#pragma once
#include "Weapon.h"

class AxeWeapon : public Weapon
{
private:
	float projectileSpeed_;
	float spinRadius_;

public:
	AxeWeapon();
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
};

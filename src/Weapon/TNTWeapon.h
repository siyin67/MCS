#pragma once
#include "Weapon.h"

class TNTWeapon : public Weapon
{
private:
	float explosionRadius_;
	float projectileSpeed_;
	int throwCount_;

public:
	TNTWeapon();
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
};
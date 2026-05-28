#pragma once
#include "weapon.h"
#include <random>

class SeaHeartWeapon : public Weapon
{
private:
	int dropletCount_;
	int bounceCount_;
	float dropletSpeed_;

public:
	SeaHeartWeapon();

	void update(float deltaTime) override;
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
};

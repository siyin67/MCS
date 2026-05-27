#pragma once
#include "Weapon.h"

class DragonBreathWeapon : public Weapon
{
private:
	float radius_;
	float areaDuration_;
	int maxLayers_;
	int activeCount_;

public:
	DragonBreathWeapon();
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
	void onAreaDestroyed() { if (activeCount_ > 0) activeCount_--; }
};

#pragma once
#include "Weapon.h"

class WeaponManager;

class ClockWeapon : public Weapon
{
private:
	WeaponManager *weaponManager_ = nullptr;
	bool firstTrigger_ = true;

public:
	ClockWeapon();
	void setWeaponManager(WeaponManager *wm) { weaponManager_ = wm; }
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
};

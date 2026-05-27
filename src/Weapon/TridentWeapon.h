#pragma once
#include "Weapon.h"

class TridentWeapon : public Weapon
{
private:
	float projectileSpeed_;
	int maxOnField_;  // 场上最大三叉戟数量
	int activeCount_; // 当前场上数量

public:
	TridentWeapon();
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void levelUp() override;
	void onTridentDestroyed() { if (activeCount_ > 0) activeCount_--; }
};
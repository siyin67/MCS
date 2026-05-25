#pragma once
#include "Weapon.h"

class SwordWeapon : public Weapon
{
public:
    SwordWeapon();
    void attack(const sf::Vector2f &playerPos,
                const sf::Vector2f &attackDir,
                std::vector<std::unique_ptr<Monster>> &monsters) override;
};
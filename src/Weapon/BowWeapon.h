// BowWeapon.h
#pragma once
#include "Weapon.h"

class BowWeapon : public Weapon
{
private:
    float projectileSpeed_;
    float projectileLifetime_;
    std::string projectileId_;

public:
    BowWeapon();
    void attack(const sf::Vector2f &playerPos,
                const sf::Vector2f &attackDir,
                std::vector<std::unique_ptr<Monster>> &monsters) override;
};
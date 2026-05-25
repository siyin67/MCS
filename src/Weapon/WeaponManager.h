#pragma once
#include <vector>
#include <memory>
#include "Weapon.h"

class WeaponManager
{
private:
    std::vector<std::unique_ptr<Weapon>> weapons_;
    int maxWeapons_ = 6;

public:
    void addWeapon(std::unique_ptr<Weapon> weapon);
    void upgradeWeapon(int index);
    void update(float deltaTime, const sf::Vector2f &playerPos,
                const sf::Vector2f &attackDir,
                std::vector<std::unique_ptr<Monster>> &monsters);

    std::vector<std::unique_ptr<Weapon>> &getWeapons() { return weapons_; }
    int getWeaponCount() const { return weapons_.size(); }
    void clear();
};
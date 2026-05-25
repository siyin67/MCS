#include "WeaponManager.h"

void WeaponManager::addWeapon(std::unique_ptr<Weapon> weapon)
{
    if (weapons_.size() < maxWeapons_)
    {
        weapons_.push_back(std::move(weapon));
    }
}

void WeaponManager::upgradeWeapon(int index)
{
    if (index >= 0 && index < (int)weapons_.size())
    {
        weapons_[index]->levelUp();
    }
}

void WeaponManager::update(float deltaTime, const sf::Vector2f &playerPos,
                           const sf::Vector2f &attackDir,
                           std::vector<std::unique_ptr<Monster>> &monsters)
{
    for (auto &weapon : weapons_)
    {
        weapon->update(deltaTime);
        weapon->attack(playerPos, attackDir, monsters);
    }
}

void WeaponManager::clear()
{
    weapons_.clear();
}
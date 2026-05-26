#include "WeaponManager.h"
#include "SwordWeapon.h"

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
void WeaponManager::render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const
{
    for (const auto &weapon : weapons_)
    {
        // 尝试转换为 SwordWeapon
        if (auto *sword = dynamic_cast<SwordWeapon *>(weapon.get()))
        {
            sword->render(window, playerPos);
        }
    }
}

void WeaponManager::clear()
{
    weapons_.clear();
}
#include "SwordWeapon.h"
#include "../Monster/Monster.h"
#include <cmath>
#include <iostream>

SwordWeapon::SwordWeapon()
{
    name_ = "Iron Sword";
    textureId_ = "iron_sword";
    baseDamage_ = 20;
    damage_ = 20;
    attackCooldown_ = 0.8f;
    range_ = 180.0f;
    level_ = 1;
}

void SwordWeapon::attack(const sf::Vector2f &playerPos,
                         const sf::Vector2f &attackDir,
                         std::vector<std::unique_ptr<Monster>> &monsters)
{
    if (!canAttack())
        return;

    for (auto &monster : monsters)
    {
        if (!monster->isAlive())
            continue;

        float dx = monster->getPosition().x - playerPos.x;
        float dy = monster->getPosition().y - playerPos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > range_)
            continue;

        // 判断怪物是否在攻击方向的180度扇面内
        if (distance > 0.01f)
        {
            sf::Vector2f toMonster(dx / distance, dy / distance);
            float dot = attackDir.x * toMonster.x + attackDir.y * toMonster.y;
            if (dot < 0.0f) // 夹角 > 90度，不在180度扇面内
                continue;
        }

        monster->takeDamage(damage_);
    }

    resetCooldown();
}
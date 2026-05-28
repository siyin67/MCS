// BowWeapon.cpp
#include "BowWeapon.h"
#include "Projectile/Arrow.h"
#include "Sound/SoundManager.h"

BowWeapon::BowWeapon()
{
    name_ = "Bow";
    textureId_ = "bow";
    baseDamage_ = 15;
    damage_ = 15;
    attackCooldown_ = 0.6f;
    range_ = 400.0f;
    level_ = 1;

    projectileSpeed_ = 1600.0f;
    projectileLifetime_ = 1.5f;
    projectileId_ = "arrow";
}

void BowWeapon::attack(const sf::Vector2f &playerPos,
                       const sf::Vector2f &attackDir,
                       std::vector<std::unique_ptr<Monster>> &monsters)
{
    if (!canAttack())
        return;

    // 播放射箭音效
    SoundManager::getInstance().playSFX("bow_shoot");

    // 创建箭矢
    auto arrow = std::make_unique<Arrow>();
    arrow->setPosition(playerPos);
    arrow->setVelocity(attackDir * projectileSpeed_);
    arrow->setDamage(damage_);
    arrow->setLifetime(projectileLifetime_);
    arrow->setFriendly(true);

    // 使用基类方法发射（不再需要单独的回调）
    addProjectile(std::move(arrow));

    resetCooldown();
}
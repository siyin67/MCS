// BowWeapon.cpp
#include "BowWeapon.h"
#include "Projectile/Arrow.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"

BowWeapon::BowWeapon()
{
    auto cfg = loadJsonConfig("assets/config/weapon/bow.json");
    name_ = cfg.value("name", "Bow");
    textureId_ = cfg.value("textureId", "bow");
    baseDamage_ = cfg.value("baseDamage", 15);
    damage_ = cfg.value("damage", 15);
    attackCooldown_ = cfg.value("attackCooldown", 0.6f);
    range_ = cfg.value("range", 400.0f);
    level_ = 1;
    levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});

    projectileSpeed_ = cfg.value("projectileSpeed", 1600.0f);
    projectileLifetime_ = cfg.value("projectileLifetime", 1.5f);
    projectileId_ = cfg.value("projectileId", "arrow");
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
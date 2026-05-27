#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <Projectile/projectile.h>

class Monster;
class Projectile;
class Weapon
{
protected:
    int baseDamage_;
    int damage_;
    int level_;
    float attackCooldown_;
    float currentCooldown_;
    float range_; // 攻击范围
    std::string name_;
    std::string textureId_;
    std::function<void(std::unique_ptr<Projectile>)> addProjectileCallback_;

public:
    Weapon();
    virtual ~Weapon() = default;

    virtual void update(float deltaTime);
    virtual void attack(const sf::Vector2f &playerPos,
                        const sf::Vector2f &attackDir,
                        std::vector<std::unique_ptr<Monster>> &monsters) = 0;
    virtual void levelUp();

    bool canAttack() const;
    void resetCooldown();
    void clearCooldown() { currentCooldown_ = 0.0f; }
    void dealDamage(Monster &monster); // 直接伤害单个怪物

    // Getter
    int getDamage() const { return damage_; }
    int getBaseDamage() const { return baseDamage_; }
    int getLevel() const { return level_; }
    float getAttackCooldown() const { return attackCooldown_; }
    float getCurrentCooldown() const { return currentCooldown_; }
    float getRange() const { return range_; }
    const std::string &getName() const { return name_; }
    const std::string &getTextureId() const { return textureId_; }

    // Setter
    void setName(const std::string &name) { name_ = name; }
    void setTextureId(const std::string &id) { textureId_ = id; }
    void setRange(float range) { range_ = range; }

    void setAddProjectileCallback(std::function<void(std::unique_ptr<Projectile>)> callback)
    {
        addProjectileCallback_ = callback;
    }
    bool hasCallback() const { return addProjectileCallback_ != nullptr; }

protected:
    // 新增：发射投射物的辅助方法
    void addProjectile(std::unique_ptr<Projectile> projectile)
    {
        if (addProjectileCallback_)
        {
            addProjectileCallback_(std::move(projectile));
        }
    }
};
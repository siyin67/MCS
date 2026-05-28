#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <vector>
#include "collision/Collision.h"

// 前向声明
class Player;
#include "Projectile/Projectile.h"
#include "Buff/Buff.h"

class Monster : public ICollidable
{
public:
    static inline std::vector<std::unique_ptr<Monster>> pendingSpawns;
    static inline std::vector<std::unique_ptr<Projectile>> pendingProjectiles;
    static inline std::function<void(sf::Vector2f, float, float, BuffType, int)> spawnAreaCallback; // pos, radius, duration, buffType, value
    static inline sf::Vector2f pendingPlayerForce; // 怪物对玩家的外力累积

protected:
    sf::RectangleShape shape_;
    sf::Vector2f position_;
    float speed_;
    int health_;
    int maxHealth_;
    int damage_;
    int expReward_;
    sf::Color originalColor_;
    float hurtTimer_;
    BuffManager buffManager_;
    // 冻结
    bool frozen_ = false;
    float frozenTimer_ = 0.0f;
    // 技能相关
    float skillCooldown_;
    float skillTimer_;
    std::function<void(Monster &, const sf::Vector2f &)> skillFunc_;

public:
    Monster();
    virtual ~Monster() = default;

    virtual void playHitSound() = 0;
    virtual void playDeathSound() = 0;
    // 基础更新
    virtual void update(float deltaTime, const sf::Vector2f &playerPos,
                        const std::vector<std::unique_ptr<Monster>> &allMonsters);
    virtual void render(sf::RenderWindow &window);
    virtual void takeDamage(int damage);
    void takeDamageSilent(int damage)
    {
        health_ -= damage;
        if (health_ < 0) health_ = 0;
    }

    // 碰撞接口：怪物与玩家碰撞时的行为，子类可重写
    virtual void onCollideWithPlayer(Player &player, const sf::Vector2f &knockbackDir);

    // 技能接口
    virtual void setSkill(std::function<void(Monster &, const sf::Vector2f &)> skill, float cooldown);
    virtual void updateSkill(float deltaTime, const sf::Vector2f &playerPos);
    virtual void useSkill(const sf::Vector2f &playerPos);

    // 位置操作
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

    // 冻结
    void freeze(float duration) { frozen_ = true; frozenTimer_ = duration; }
    bool isFrozen() const { return frozen_; }
    virtual bool isProjectileImmune() const { return false; }

    // 属性获取
    int getHealth() const { return health_; }
    int getDamage() const { return static_cast<int>(damage_ * buffManager_.getDamageMultiplier()); }
    int getBaseDamage() const { return damage_; }
    int getExpReward() const { return expReward_; }
    float getEffectiveSpeed() const { return speed_ * buffManager_.getSpeedMultiplier(); }
    BuffManager &getBuffManager() { return buffManager_; }
    bool isAlive() const { return health_ > 0; }
    void setAlive(bool alive)
    {
        if (!alive)
            health_ = 0;
    }
    void addHealth(int amount)
    {
        health_ += amount;
        maxHealth_ += amount;
    }
    void heal(int amount)
    {
        health_ = std::min(maxHealth_, health_ + amount);
    }

    // 特效标记（供外部使用）
    bool shouldSpawnEffect_ = false;
    sf::Vector2f effectPosition_;

    // ICollidable 接口实现
    sf::FloatRect getGlobalBounds() const override;
    std::vector<sf::Vector2f> getVertices() const override;
    CollisionType getType() const override;
};
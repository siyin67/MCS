#pragma once
#include <SFML/Graphics.hpp>
#include "collision/Collision.h"

// 前向声明
class Monster;

class Player : public ICollidable
{
private:
    sf::RectangleShape shape_;
    float speed_;
    int health_;
    int maxHealth_;

    sf::ConvexShape arrow_;
    sf::Vector2f attackDirection_;

    // ---------- 碰撞相关 ----------
    float invincibleTimer_;       // 无敌时间倒计时（秒）
    float invincibleDuration_;    // 无敌持续总时长（秒）
    bool  isInvincible_;          // 当前是否处于无敌状态

    sf::Vector2f knockbackVelocity_; // 击退速度
    float knockbackDecay_;           // 击退衰减系数

    float hurtFlashTimer_;       // 受伤闪烁计时器
    float hurtFlashInterval_;    // 闪烁间隔
    bool  showHurtColor_;        // 当前是否显示受伤颜色

    sf::Color originalColor_;    // 原始填充色（用于恢复）

public:
    Player();

    void update(float deltaTime);
    void takeDamage(int damage, const sf::Vector2f &knockbackDir = {0, 0});
    void heal(int amount);

    // 碰撞接口：玩家与怪物碰撞时的行为
    void onCollideWithMonster(Monster &monster, const sf::Vector2f &knockbackDir);

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

    void render(sf::RenderWindow &window);

    int getHealth() const;
    bool isAlive() const;
    float getHealthPercent() const;
    bool getIsInvincible() const { return isInvincible_; }

    void updateAttackDirection(const sf::RenderWindow &window);
    sf::Vector2f getAttackDirection() const { return attackDirection_; }

    // ICollidable 接口实现
    sf::FloatRect getGlobalBounds() const override;
    std::vector<sf::Vector2f> getVertices() const override;
    CollisionType getType() const override;
};
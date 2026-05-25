#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include "collision/Collision.h"

// 前向声明
class Player;

class Monster : public ICollidable
{
protected:
    sf::RectangleShape shape_;
    sf::Vector2f position_;
    float speed_;
    int health_;
    int maxHealth_;
    int damage_;
    int expReward_;

    // 技能相关
    float skillCooldown_;
    float skillTimer_;

public:
    Monster();
    virtual ~Monster() = default;
    
    // 基础更新
    virtual void update(float deltaTime, const sf::Vector2f &playerPos,
                        const std::vector<std::unique_ptr<Monster>> &allMonsters);
    virtual void render(sf::RenderWindow &window);
    virtual void takeDamage(int damage);

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

    // 属性获取
    int getHealth() const { return health_; }
    int getDamage() const { return damage_; }
    int getExpReward() const { return expReward_; }
    bool isAlive() const { return health_ > 0; }
    void setAlive(bool alive)
    {
        if (!alive)
            health_ = 0;
    }

    // 特效标记（供外部使用）
    bool shouldSpawnEffect_ = false;
    sf::Vector2f effectPosition_;

    // ICollidable 接口实现
    sf::FloatRect getGlobalBounds() const override;
    std::vector<sf::Vector2f> getVertices() const override;
    CollisionType getType() const override;
};
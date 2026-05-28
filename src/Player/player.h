#pragma once
#include <SFML/Graphics.hpp>
#include "collision/Collision.h"
#include "Weapon/WeaponManager.h"
#include "Buff/Buff.h"

// 前向声明
class Monster;

class Player : public ICollidable
{
private:
    sf::RectangleShape shape_;
    float speed_;
    int health_;
    int maxHealth_;
    BuffManager buffManager_;

    sf::ConvexShape arrow_;
    sf::Vector2f attackDirection_;

    // ---------- 碰撞相关 ----------
    float invincibleTimer_;
    float invincibleDuration_;
    bool isInvincible_;

    sf::Vector2f knockbackVelocity_;
    float knockbackDecay_;

    float hurtFlashTimer_;
    float hurtFlashInterval_;
    bool showHurtColor_;

    sf::Color originalColor_;
    sf::Vector2f externalForce_;
    WeaponManager weaponManager_;

    int currentXP_ = 0;
    int level_ = 1;

public:
    Player();

    void update(float deltaTime);
    void takeDamage(int damage, const sf::Vector2f &knockbackDir = {0, 0});
    void takeEnvironmentalDamage(int damage);
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

    int addXP(int amount);
    int getXP() const { return currentXP_; }
    int getLevel() const { return level_; }
    int getXPToNext() const { return level_ * 100; }
    float getXPPercent() const;
    void applyExternalForce(const sf::Vector2f &force) { externalForce_ += force; }

    void updateAttackDirection(const sf::RenderWindow &window);
    sf::Vector2f getAttackDirection() const { return attackDirection_; }

    // ICollidable 接口实现
    sf::FloatRect getGlobalBounds() const override;
    std::vector<sf::Vector2f> getVertices() const override;
    CollisionType getType() const override;

    WeaponManager &getWeaponManager() { return weaponManager_; }
    const WeaponManager& getWeaponManager() const { return weaponManager_; }
    void addWeapon(std::unique_ptr<Weapon> weapon);
    void updateWeapons(float deltaTime, const sf::Vector2f &playerPos,
                       const sf::Vector2f &attackDir,
                       std::vector<std::unique_ptr<Monster>> &monsters);

    // Buff
    BuffManager &getBuffManager() { return buffManager_; }
    const BuffManager &getBuffManager() const { return buffManager_; }
    float getEffectiveSpeed() const { return speed_ * buffManager_.getSpeedMultiplier(); }
    float getEffectiveDamage(float baseDamage) const { return baseDamage * buffManager_.getDamageMultiplier(); }
};

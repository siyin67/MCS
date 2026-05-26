#pragma once
#include "Weapon.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct Shield
{
    sf::Sprite sprite;      // 盾牌贴图
    float angle;            // 当前角度（弧度）
    float cooldown;         // 伤害冷却计时器
    bool canDamage;         // 是否可造成伤害
};

class ShieldWeapon : public Weapon
{
private:
    std::vector<Shield> shields_;
    float radius_;           // 环绕半径
    float rotationSpeed_;    // 旋转速度（弧度/秒）
    int maxShields_;         // 最大盾牌数量
    float hitCooldown_;      // 命中后冷却时间
    sf::Texture shieldTex_;  // 盾牌纹理

    void updateShields(float deltaTime, const sf::Vector2f& playerPos,
                       std::vector<std::unique_ptr<Monster>>& monsters);
    void createShield();

public:
    ShieldWeapon();
    void attack(const sf::Vector2f& playerPos,
                const sf::Vector2f& attackDir,
                std::vector<std::unique_ptr<Monster>>& monsters) override;
    void update(float deltaTime) override;
    void levelUp() override;
    void render(sf::RenderWindow& window, const sf::Vector2f& playerPos) const;
};
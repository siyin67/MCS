#pragma once
#include "Weapon.h"
#include <SFML/Graphics.hpp>
#include <vector>

class SwordWeapon : public Weapon
{
private:
    // 攻击动画
    float attackTimer_;
    bool isAttacking_;
    sf::Vector2f attackPos_;
    sf::Vector2f lastAttackDir_;

    // 特效动画（用纹理 + 矩形）
    std::vector<sf::Texture> slashTextures_;
    int currentFrame_;
    float frameTimer_;
    bool isSlashing_;
    sf::RectangleShape slashShape_; // 改用 RectangleShape

public:
    SwordWeapon();
    void attack(const sf::Vector2f &playerPos,
                const sf::Vector2f &attackDir,
                std::vector<std::unique_ptr<Monster>> &monsters) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const;
};
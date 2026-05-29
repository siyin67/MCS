#include "SwordWeapon.h"
#include "../Monster/Monster.h"
#include "../Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>
#include <iostream>

SwordWeapon::SwordWeapon()
{
    auto cfg = loadJsonConfig("assets/config/weapon/sword.json");
    name_ = cfg.value("name", "Iron Sword");
    textureId_ = cfg.value("textureId", "iron_sword");
    baseDamage_ = cfg.value("baseDamage", 20);
    damage_ = cfg.value("damage", 10);
    attackCooldown_ = cfg.value("attackCooldown", 0.8f);
    range_ = cfg.value("range", 280.0f);
    level_ = 1;
    levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});

    attackTimer_ = 0.0f;
    isAttacking_ = false;
    lastAttackDir_ = {1.0f, 0.0f};

    currentFrame_ = 0;
    frameTimer_ = 0.0f;
    isSlashing_ = false;

    // 加载攻击特效贴图
    for (int i = 0; i <= 7; i++)
    {
        std::string path = "assets/images/effects/sword/sweep_" + std::to_string(i) + ".png";
        sf::Texture tex;
        if (tex.loadFromFile(path))
        {
            slashTextures_.push_back(tex);
        }
    }

    // 设置矩形形状
    if (!slashTextures_.empty())
    {
        slashShape_.setTexture(&slashTextures_[0]);
    }
    slashShape_.setSize({32, 32});
    slashShape_.setOrigin({16, 16}); // 中心点
}

void SwordWeapon::update(float deltaTime)
{
    Weapon::update(deltaTime);

    if (isAttacking_)
    {
        attackTimer_ -= deltaTime;
        if (attackTimer_ <= 0.0f)
        {
            isAttacking_ = false;
        }
    }

    if (isSlashing_)
    {
        frameTimer_ += deltaTime;
        if (frameTimer_ >= 0.05f)
        {
            frameTimer_ = 0.0f;
            currentFrame_++;
            if (currentFrame_ >= (int)slashTextures_.size())
            {
                isSlashing_ = false;
            }
            else
            {
                // 更新纹理
                slashShape_.setTexture(&slashTextures_[currentFrame_]);
            }
        }
    }
}

void SwordWeapon::attack(const sf::Vector2f &playerPos,
                         const sf::Vector2f &attackDir,
                         std::vector<std::unique_ptr<Monster>> &monsters)
{
    if (!canAttack())
        return;

    lastAttackDir_ = attackDir;

    isAttacking_ = true;
    attackTimer_ = 0.2f;
    attackPos_ = playerPos;

    isSlashing_ = true;
    currentFrame_ = 0;
    frameTimer_ = 0.0f;
    slashShape_.setTexture(&slashTextures_[0]);

    SoundManager::getInstance().playSFX("sword_swing");

    for (auto &monster : monsters)
    {
        if (!monster->isAlive())
            continue;

        float dx = monster->getPosition().x - playerPos.x;
        float dy = monster->getPosition().y - playerPos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > range_)
            continue;

        if (distance > 0.01f)
        {
            sf::Vector2f toMonster(dx / distance, dy / distance);
            float dot = attackDir.x * toMonster.x + attackDir.y * toMonster.y;
            if (dot < 0.0f)
                continue;
        }

        monster->takeDamage(damage_);
    }

    resetCooldown();
}

void SwordWeapon::render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const
{
    if (!isSlashing_ || slashTextures_.empty())
        return;

    sf::RectangleShape shape = slashShape_;
    shape.setTexture(&slashTextures_[currentFrame_]);

    static constexpr float REFERENCE_RANGE = 100.0f;
    float scale = (range_ / REFERENCE_RANGE) * 2.0f;
    float size = 32.0f * scale;
    shape.setSize({size, size});

    // 缩放后重新设置原点为中心
    shape.setOrigin({size / 2.0f, size / 2.0f});

    // 计算攻击方向角度
    float angleRad = std::atan2(lastAttackDir_.y, lastAttackDir_.x);
    float angleDeg = angleRad * 180 / 3.14159f;

    // 计算偏移距离（特效半径的 70%，让特效在玩家前方）
    float offset = size * 0.7f;
    sf::Vector2f offsetVec(std::cos(angleRad) * offset, std::sin(angleRad) * offset);

    // 位置 = 玩家中心 + 攻击方向偏移
    shape.setPosition(playerPos + offsetVec);

    // 旋转特效（贴图默认向右，+90度变成向上）
    shape.setRotation(sf::degrees(angleDeg + 90.0f));

    window.draw(shape);
}
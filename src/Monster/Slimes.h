
#pragma once
#include "Monster.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>

// ============================================================================
// 小史莱姆 (SmallSlime) — 最先定义，因为 BigSlime/KingSlime 依赖它
// ============================================================================
class SmallSlime : public Monster
{
private:
    // 冲刺相关
    sf::Vector2f sprintVelocity_{0.0f, 0.0f};
    float sprintDecay_ = 4.0f;          // 冲刺衰减系数
    float sprintCooldown_ = 2.0f;       // 冲刺冷却时间（秒）
    float sprintTimer_ = 0.0f;          // 冲刺冷却计时器
    float sprintSpeed_ = 280.0f;        // 冲刺初始速率
    float sprintTriggerRange_ = 195.0f; // 触发冲刺的玩家距离阈值 (原130 *1.5)

    // 受伤闪烁反馈
    float hurtTimer_ = 0.0f;                            // 受伤红色闪烁剩余时间
    static constexpr float HURT_FLASH_DURATION = 0.15f; // 闪烁持续时间

    // 常量
    static constexpr float MAP_SIZE = 5000.0f;

public:
    SmallSlime()
    {
        speed_ = 60.0f;
        health_ = maxHealth_ = 50;
        damage_ = 1;
        expReward_ = 10;

        shape_.setSize({32.0f, 32.0f});
        shape_.setOrigin({16.0f, 16.0f}); // 中心原点，边界限制用半宽

        if (ResourceManager::GetInstance().HasTexture("slime"))
        {
            shape_.setTexture(&ResourceManager::GetInstance().GetTexture("slime"));
        }
        shape_.setFillColor(sf::Color::White);
        originalColor_ = sf::Color::White;
    }

    ~SmallSlime() override = default;

    void playHitSound() override
    {
        SoundManager::getInstance().playSFX("zombie_hurt");
    }
    void playDeathSound() override
    {
        SoundManager::getInstance().playSFX("zombie_death");
    }

    void update(float deltaTime, const sf::Vector2f &playerPos,
                const std::vector<std::unique_ptr<Monster>> &allMonsters) override
    {
        // 受伤闪烁恢复
        if (hurtTimer_ > 0.0f)
        {
            hurtTimer_ -= deltaTime;
            if (hurtTimer_ <= 0.0f)
                shape_.setFillColor(sf::Color::White);
        }

        // 判断是否处于冲刺状态
        float sprintLen = std::sqrt(sprintVelocity_.x * sprintVelocity_.x +
                                    sprintVelocity_.y * sprintVelocity_.y);

        if (sprintLen < 1.0f)
        {
            // ---- 普通状态：追踪 + 分离 ----
            Monster::update(deltaTime, playerPos, allMonsters);

            // 冲刺冷却递减
            sprintTimer_ -= deltaTime;
            if (sprintTimer_ <= 0.0f)
            {
                // 检查与玩家距离
                sf::Vector2f toPlayer = playerPos - position_;
                float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
                if (dist <= sprintTriggerRange_ && dist > 0.01f)
                {
                    // 触发冲刺
                    sf::Vector2f dir = toPlayer / dist;
                    sprintVelocity_ = dir * sprintSpeed_;
                    sprintTimer_ = sprintCooldown_;
                }
            }
        }
        else
        {
            // ---- 冲刺状态：沿冲刺方向移动 ----
            position_ += sprintVelocity_ * deltaTime;
            shape_.setPosition(position_);

            // 指数衰减
            sprintVelocity_ *= (1.0f - sprintDecay_ * deltaTime);
            if (std::abs(sprintVelocity_.x) < 1.0f && std::abs(sprintVelocity_.y) < 1.0f)
                sprintVelocity_ = {0.0f, 0.0f};
        }

        // 边界限制（原点为中心，半宽 = 16）
        float halfW = shape_.getSize().x / 2.0f;
        float halfH = shape_.getSize().y / 2.0f;
        position_.x = std::max(halfW, std::min(position_.x, MAP_SIZE - halfW));
        position_.y = std::max(halfH, std::min(position_.y, MAP_SIZE - halfH));
        shape_.setPosition(position_);
    }

    void takeDamage(int damage) override
    {
        health_ -= damage;
        if (health_ < 0)
            health_ = 0;
        playHitSound();
        // 短暂红色闪烁
        shape_.setFillColor(sf::Color(255, 80, 80));
        hurtTimer_ = HURT_FLASH_DURATION;
    }

    void onCollideWithPlayer(Player &player, const sf::Vector2f &knockbackDir) override
    {
        // 冲刺碰撞时停止冲刺，不额外造成伤害（系统已处理）
        float sprintLen = std::sqrt(sprintVelocity_.x * sprintVelocity_.x +
                                    sprintVelocity_.y * sprintVelocity_.y);
        if (sprintLen >= 1.0f)
            sprintVelocity_ = {0.0f, 0.0f};
    }

    void render(sf::RenderWindow &window) override
    {
        Monster::render(window);
    }
};

// ============================================================================
// 大史莱姆 (BigSlime) — 第二个定义，KingSlime 依赖它
// ============================================================================
class BigSlime : public Monster
{
private:
    // 冲刺相关
    sf::Vector2f sprintVelocity_{0.0f, 0.0f};
    float sprintDecay_ = 4.0f;
    float sprintCooldown_ = 2.5f;
    float sprintTimer_ = 0.0f;
    float sprintSpeed_ = 320.0f;
    float sprintTriggerRange_ = 280.0f; // (原140 *2)

    // 分裂标志
    bool hasSplit_ = false;

    // 受伤闪烁
    float hurtTimer_ = 0.0f;
    static constexpr float HURT_FLASH_DURATION = 0.15f;

    static constexpr float MAP_SIZE = 5000.0f;

public:
    BigSlime()
    {
        speed_ = 50.0f;
        health_ = maxHealth_ = 100;
        damage_ = 2;
        expReward_ = 20;

        shape_.setSize({48.0f, 48.0f});
        shape_.setOrigin({24.0f, 24.0f});

        if (ResourceManager::GetInstance().HasTexture("slime"))
        {
            shape_.setTexture(&ResourceManager::GetInstance().GetTexture("slime"));
        }
        shape_.setFillColor(sf::Color::White);
        originalColor_ = sf::Color::White;

        // 技能计时器初始化为随机冷却，避免第一帧立即触发
        skillCooldown_ = 10.0f + static_cast<float>(rand() % 11); // 10~20 秒
        skillTimer_ = skillCooldown_;
    }

    ~BigSlime() override = default;

    void playHitSound() override
    {
        SoundManager::getInstance().playSFX("zombie_hurt");
    }
    void playDeathSound() override
    {
        SoundManager::getInstance().playSFX("zombie_death");
    }

    // ---- 技能：召唤小史莱姆 ----
    void updateSkill(float deltaTime, const sf::Vector2f &playerPos) override
    {
        skillTimer_ -= deltaTime;
        if (skillTimer_ <= 0.0f)
            useSkill(playerPos);
    }

    void useSkill(const sf::Vector2f &playerPos) override
    {
        // 随机召唤 1~2 只小史莱姆
        int count = 1 + rand() % 2;
        for (int i = 0; i < count; ++i)
        {
            float offsetX = static_cast<float>((rand() % 61) - 30); // ±30
            float offsetY = static_cast<float>((rand() % 31) - 15); // ±15
            auto child = std::make_unique<SmallSlime>();
            sf::Vector2f spawnPos = position_ + sf::Vector2f(offsetX, offsetY);
            // 钳制在地图内
            spawnPos.x = std::max(16.0f, std::min(spawnPos.x, MAP_SIZE - 16.0f));
            spawnPos.y = std::max(16.0f, std::min(spawnPos.y, MAP_SIZE - 16.0f));
            child->setPosition(spawnPos);
            Monster::pendingSpawns.push_back(std::move(child));
        }
        // 重新随机冷却
        skillCooldown_ = 10.0f + static_cast<float>(rand() % 11);
        skillTimer_ = skillCooldown_;
    }

    void update(float deltaTime, const sf::Vector2f &playerPos,
                const std::vector<std::unique_ptr<Monster>> &allMonsters) override
    {
        // 受伤闪烁恢复
        if (hurtTimer_ > 0.0f)
        {
            hurtTimer_ -= deltaTime;
            if (hurtTimer_ <= 0.0f)
                shape_.setFillColor(sf::Color::White);
        }

        // 技能冷却更新
        updateSkill(deltaTime, playerPos);

        // 冲刺状态判断
        float sprintLen = std::sqrt(sprintVelocity_.x * sprintVelocity_.x +
                                    sprintVelocity_.y * sprintVelocity_.y);

        if (sprintLen < 1.0f)
        {
            // 普通状态
            Monster::update(deltaTime, playerPos, allMonsters);

            sprintTimer_ -= deltaTime;
            if (sprintTimer_ <= 0.0f)
            {
                sf::Vector2f toPlayer = playerPos - position_;
                float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
                if (dist <= sprintTriggerRange_ && dist > 0.01f)
                {
                    sprintVelocity_ = (toPlayer / dist) * sprintSpeed_;
                    sprintTimer_ = sprintCooldown_;
                }
            }
        }
        else
        {
            // 冲刺状态
            position_ += sprintVelocity_ * deltaTime;
            shape_.setPosition(position_);

            sprintVelocity_ *= (1.0f - sprintDecay_ * deltaTime);
            if (std::abs(sprintVelocity_.x) < 1.0f && std::abs(sprintVelocity_.y) < 1.0f)
                sprintVelocity_ = {0.0f, 0.0f};
        }

        // 边界限制（半宽 = 24）
        float halfW = shape_.getSize().x / 2.0f;
        float halfH = shape_.getSize().y / 2.0f;
        position_.x = std::max(halfW, std::min(position_.x, MAP_SIZE - halfW));
        position_.y = std::max(halfH, std::min(position_.y, MAP_SIZE - halfH));
        shape_.setPosition(position_);
    }

    void takeDamage(int damage) override
    {
        health_ -= damage;
        if (health_ < 0)
            health_ = 0;

        // 半血分裂
        if (health_ <= maxHealth_ / 2 && health_ > 0 && !hasSplit_)
        {
            hasSplit_ = true;
            // 分裂为两只小史莱姆
            for (int i = 0; i < 2; ++i)
            {
                float offsetX = (i == 0) ? -30.0f : 30.0f;
                auto child = std::make_unique<SmallSlime>();
                sf::Vector2f spawnPos = position_ + sf::Vector2f(offsetX, 0.0f);
                spawnPos.x = std::max(16.0f, std::min(spawnPos.x, MAP_SIZE - 16.0f));
                spawnPos.y = std::max(16.0f, std::min(spawnPos.y, MAP_SIZE - 16.0f));
                child->setPosition(spawnPos);
                Monster::pendingSpawns.push_back(std::move(child));
            }
            // 自身死亡
            health_ = 0;
        }

        // 受伤闪烁
        if (health_ > 0)
        {
            playHitSound();
            shape_.setFillColor(sf::Color(255, 80, 80));
            hurtTimer_ = HURT_FLASH_DURATION;
        }
    }

    void onCollideWithPlayer(Player &player, const sf::Vector2f &knockbackDir) override
    {
        float sprintLen = std::sqrt(sprintVelocity_.x * sprintVelocity_.x +
                                    sprintVelocity_.y * sprintVelocity_.y);
        if (sprintLen >= 1.0f)
            sprintVelocity_ = {0.0f, 0.0f};
    }

    void render(sf::RenderWindow &window) override
    {
        Monster::render(window);
    }
};

// ============================================================================
// 史莱姆王 (KingSlime) — 最后定义，依赖 BigSlime
// ============================================================================
class KingSlime : public Monster
{
private:
    // 冲刺相关
    sf::Vector2f sprintVelocity_{0.0f, 0.0f};
    float sprintDecay_ = 4.0f;
    float sprintCooldown_ = 3.0f;
    float sprintTimer_ = 0.0f;
    float sprintSpeed_ = 350.0f;
    float sprintTriggerRange_ = 375.0f; // (原150 *2.5)

    // 分裂标志
    bool hasSplit_ = false;

    // 受伤闪烁
    float hurtTimer_ = 0.0f;
    static constexpr float HURT_FLASH_DURATION = 0.15f;

    static constexpr float MAP_SIZE = 5000.0f;

public:
    KingSlime()
    {
        speed_ = 40.0f;
        health_ = maxHealth_ = 200;
        damage_ = 5;
        expReward_ = 50;

        shape_.setSize({64.0f, 64.0f});
        shape_.setOrigin({32.0f, 32.0f});

        if (ResourceManager::GetInstance().HasTexture("slime"))
        {
            shape_.setTexture(&ResourceManager::GetInstance().GetTexture("slime"));
        }
        shape_.setFillColor(sf::Color::White);
        originalColor_ = sf::Color::White;

        // 技能计时器初始化为随机冷却，避免第一帧立即触发
        skillCooldown_ = 10.0f + static_cast<float>(rand() % 11); // 10~20 秒
        skillTimer_ = skillCooldown_;
    }

    ~KingSlime() override = default;

    void playHitSound() override
    {
        SoundManager::getInstance().playSFX("zombie_hurt");
    }
    void playDeathSound() override
    {
        SoundManager::getInstance().playSFX("zombie_death");
    }

    // ---- 技能：召唤大史莱姆 ----
    void updateSkill(float deltaTime, const sf::Vector2f &playerPos) override
    {
        skillTimer_ -= deltaTime;
        if (skillTimer_ <= 0.0f)
            useSkill(playerPos);
    }

    void useSkill(const sf::Vector2f &playerPos) override
    {
        // 随机召唤 1~2 只大史莱姆
        int count = 1 + rand() % 2;
        for (int i = 0; i < count; ++i)
        {
            float offsetX = static_cast<float>((rand() % 81) - 40); // ±40
            float offsetY = static_cast<float>((rand() % 41) - 20); // ±20
            auto child = std::make_unique<BigSlime>();
            sf::Vector2f spawnPos = position_ + sf::Vector2f(offsetX, offsetY);
            spawnPos.x = std::max(24.0f, std::min(spawnPos.x, MAP_SIZE - 24.0f));
            spawnPos.y = std::max(24.0f, std::min(spawnPos.y, MAP_SIZE - 24.0f));
            child->setPosition(spawnPos);
            Monster::pendingSpawns.push_back(std::move(child));
        }
        // 重新随机冷却
        skillCooldown_ = 10.0f + static_cast<float>(rand() % 11);
        skillTimer_ = skillCooldown_;
    }

    void update(float deltaTime, const sf::Vector2f &playerPos,
                const std::vector<std::unique_ptr<Monster>> &allMonsters) override
    {
        // 受伤闪烁恢复
        if (hurtTimer_ > 0.0f)
        {
            hurtTimer_ -= deltaTime;
            if (hurtTimer_ <= 0.0f)
                shape_.setFillColor(sf::Color::White);
        }

        // 技能冷却更新
        updateSkill(deltaTime, playerPos);

        // 冲刺状态判断
        float sprintLen = std::sqrt(sprintVelocity_.x * sprintVelocity_.x +
                                    sprintVelocity_.y * sprintVelocity_.y);

        if (sprintLen < 1.0f)
        {
            // 普通状态
            Monster::update(deltaTime, playerPos, allMonsters);

            sprintTimer_ -= deltaTime;
            if (sprintTimer_ <= 0.0f)
            {
                sf::Vector2f toPlayer = playerPos - position_;
                float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
                if (dist <= sprintTriggerRange_ && dist > 0.01f)
                {
                    sprintVelocity_ = (toPlayer / dist) * sprintSpeed_;
                    sprintTimer_ = sprintCooldown_;
                }
            }
        }
        else
        {
            // 冲刺状态
            position_ += sprintVelocity_ * deltaTime;
            shape_.setPosition(position_);

            sprintVelocity_ *= (1.0f - sprintDecay_ * deltaTime);
            if (std::abs(sprintVelocity_.x) < 1.0f && std::abs(sprintVelocity_.y) < 1.0f)
                sprintVelocity_ = {0.0f, 0.0f};
        }

        // 边界限制（半宽 = 32）
        float halfW = shape_.getSize().x / 2.0f;
        float halfH = shape_.getSize().y / 2.0f;
        position_.x = std::max(halfW, std::min(position_.x, MAP_SIZE - halfW));
        position_.y = std::max(halfH, std::min(position_.y, MAP_SIZE - halfH));
        shape_.setPosition(position_);
    }

    void takeDamage(int damage) override
    {
        health_ -= damage;
        if (health_ < 0)
            health_ = 0;

        // 半血分裂
        if (health_ <= maxHealth_ / 2 && health_ > 0 && !hasSplit_)
        {
            hasSplit_ = true;
            // 分裂为两只大史莱姆
            for (int i = 0; i < 2; ++i)
            {
                float offsetX = (i == 0) ? -30.0f : 30.0f;
                auto child = std::make_unique<BigSlime>();
                sf::Vector2f spawnPos = position_ + sf::Vector2f(offsetX, 0.0f);
                spawnPos.x = std::max(24.0f, std::min(spawnPos.x, MAP_SIZE - 24.0f));
                spawnPos.y = std::max(24.0f, std::min(spawnPos.y, MAP_SIZE - 24.0f));
                child->setPosition(spawnPos);
                Monster::pendingSpawns.push_back(std::move(child));
            }
            // 自身死亡
            health_ = 0;
        }

        // 受伤闪烁
        if (health_ > 0)
        {
            playHitSound();
            shape_.setFillColor(sf::Color(255, 80, 80));
            hurtTimer_ = HURT_FLASH_DURATION;
        }
    }

    void onCollideWithPlayer(Player &player, const sf::Vector2f &knockbackDir) override
    {
        float sprintLen = std::sqrt(sprintVelocity_.x * sprintVelocity_.x +
                                    sprintVelocity_.y * sprintVelocity_.y);
        if (sprintLen >= 1.0f)
            sprintVelocity_ = {0.0f, 0.0f};
    }

    void render(sf::RenderWindow &window) override
    {
        Monster::render(window);
    }
};

#include "Player.h"
#include "Monster/Monster.h"
#include <cmath>
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <iostream>

Player::Player()
    : speed_(500.0f), health_(20), maxHealth_(20),
      invincibleTimer_(0.0f), invincibleDuration_(0.2f), isInvincible_(false),
      knockbackVelocity_(0.0f, 0.0f), knockbackDecay_(10.0f),
      hurtFlashTimer_(0.0f), hurtFlashInterval_(0.08f), showHurtColor_(false),
      originalColor_(sf::Color::White)
{
    std::cout << "Player created" << std::endl;
    shape_.setSize({64, 64});
    shape_.setOrigin({32, 32}); // 设置原点为中心
    shape_.setScale({0.8f, 0.8f});

    if (ResourceManager::GetInstance().HasTexture("player_skin"))
    {
        shape_.setTexture(&ResourceManager::GetInstance().GetTexture("player_skin"));
    }
    else if (ResourceManager::GetInstance().HasTexture("player"))
    {
        shape_.setTexture(&ResourceManager::GetInstance().GetTexture("player"));
    }
    else
    {
        shape_.setFillColor(sf::Color::White);
    }
    shape_.setPosition({2500, 2500});

    arrow_.setPointCount(3);
    arrow_.setPoint(0, sf::Vector2f(100, 0));  // 尖端（45像素外）
    arrow_.setPoint(1, sf::Vector2f(80, -12)); // 尾部左上
    arrow_.setPoint(2, sf::Vector2f(80, 12));  // 尾部右下
    arrow_.setFillColor(sf::Color(135, 206, 235));
    arrow_.setOutlineThickness(1);
}

void Player::update(float deltaTime)
{
    // 1. 计算移动方向
    sf::Vector2f movement(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        movement.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        movement.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        movement.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        movement.x += 1;

    // 2. 归一化对角线移动
    if (movement.x != 0 || movement.y != 0)
    {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }

    // 3. 计算新位置（含外力）
    sf::Vector2f newPos = shape_.getPosition() + movement * getEffectiveSpeed() * deltaTime + externalForce_ * deltaTime;
    externalForce_ = {0.0f, 0.0f};

    // 4. 边界限制（地图 5000x5000，玩家实际大小 64*0.8=51.2，半宽约 25.6）
    float actualSize = 64.0f * 0.8f;    // 51.2
    float halfSize = actualSize / 2.0f; // 25.6
    float minBound = halfSize;
    float maxBoundX = 5000.0f - halfSize;
    float maxBoundY = 5000.0f - halfSize;

    newPos.x = std::max(minBound, std::min(newPos.x, maxBoundX));
    newPos.y = std::max(minBound, std::min(newPos.y, maxBoundY));

    // 5. 应用位置
    shape_.setPosition(newPos);

    // ---------- 无敌帧倒计时 ----------
    if (isInvincible_)
    {
        invincibleTimer_ -= deltaTime;
        if (invincibleTimer_ <= 0.0f)
        {
            isInvincible_ = false;
            invincibleTimer_ = 0.0f;
            showHurtColor_ = false;
            // 恢复原始颜色
            if (shape_.getTexture())
                shape_.setFillColor(sf::Color::White);
            else
                shape_.setFillColor(originalColor_);
        }
        else
        {
            // 受伤闪烁：在红色和原色之间交替
            hurtFlashTimer_ -= deltaTime;
            if (hurtFlashTimer_ <= 0.0f)
            {
                hurtFlashTimer_ = hurtFlashInterval_;
                showHurtColor_ = !showHurtColor_;
                shape_.setFillColor(showHurtColor_ ? sf::Color::Red : sf::Color(255, 200, 200));
            }
        }
    }

    // ---------- 击退衰减 ----------
    if (knockbackVelocity_.x != 0.0f || knockbackVelocity_.y != 0.0f)
    {
        sf::Vector2f currentPos = shape_.getPosition();
        currentPos += knockbackVelocity_ * deltaTime;

        // 边界限制
        float actualSize = 64.0f * 0.8f;
        float halfSize = actualSize / 2.0f;
        currentPos.x = std::max(halfSize, std::min(currentPos.x, 5000.0f - halfSize));
        currentPos.y = std::max(halfSize, std::min(currentPos.y, 5000.0f - halfSize));

        shape_.setPosition(currentPos);

        // 指数衰减
        knockbackVelocity_ *= (1.0f - knockbackDecay_ * deltaTime);
        if (std::abs(knockbackVelocity_.x) < 1.0f && std::abs(knockbackVelocity_.y) < 1.0f)
        {
            knockbackVelocity_ = {0.0f, 0.0f};
        }
    }

    buffManager_.update(deltaTime, health_);
}

void Player::takeDamage(int damage, const sf::Vector2f &knockbackDir)
{
    // 无敌帧期间不受伤
    if (isInvincible_ || buffManager_.hasInvincible())
        return;

    health_ -= damage;
    if (health_ < 0)
        health_ = 0;

    // 进入无敌状态
    isInvincible_ = true;
    invincibleTimer_ = invincibleDuration_;
    hurtFlashTimer_ = 0.0f; // 立即触发闪烁
    showHurtColor_ = true;
    shape_.setFillColor(sf::Color::Red);

    // 击退：方向 * 力度
    float knockbackSpeed = 400.0f;
    knockbackVelocity_ = knockbackDir * knockbackSpeed;
    SoundManager::getInstance().playSFX("hit");
}

void Player::takeEnvironmentalDamage(int damage)
{
    // 环境伤害不触发无敌帧
    health_ -= damage;
    if (health_ < 0)
        health_ = 0;
}

void Player::onCollideWithMonster(Monster &monster, const sf::Vector2f &knockbackDir)
{
    // 玩家与怪物碰撞：受到怪物伤害 + 击退
    takeDamage(monster.getDamage(), knockbackDir);
}

void Player::heal(int amount)
{
    health_ += amount;
    if (health_ > maxHealth_)
        health_ = maxHealth_;
}

sf::Vector2f Player::getPosition() const
{
    return shape_.getPosition();
}

sf::FloatRect Player::getBounds() const
{
    return shape_.getGlobalBounds();
}

// ICollidable 接口实现
sf::FloatRect Player::getGlobalBounds() const
{
    return shape_.getGlobalBounds();
}

std::vector<sf::Vector2f> Player::getVertices() const
{
    return verticesFromBounds(getGlobalBounds());
}

CollisionType Player::getType() const
{
    return CollisionType::PLAYER;
}

void Player::render(sf::RenderWindow &window)
{
    window.draw(shape_);
    window.draw(arrow_);
}

int Player::getHealth() const
{
    return health_;
}

bool Player::isAlive() const
{
    return health_ > 0;
}

float Player::getHealthPercent() const
{
    return static_cast<float>(health_) / maxHealth_;
}

void Player::updateAttackDirection(const sf::RenderWindow &window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);

    attackDirection_ = worldMousePos - shape_.getPosition();
    float length = std::sqrt(attackDirection_.x * attackDirection_.x +
                             attackDirection_.y * attackDirection_.y);

    if (length > 0.01f)
    {
        attackDirection_.x /= length;
        attackDirection_.y /= length;
    }

    arrow_.setPosition(shape_.getPosition());

    // 计算角度（度数）
    float angleDegrees = std::atan2(attackDirection_.y, attackDirection_.x) * 180 / 3.14159f;

    arrow_.setRotation(sf::degrees(angleDegrees));
}
void Player::updateWeapons(float deltaTime, const sf::Vector2f &playerPos,
                           const sf::Vector2f &attackDir,
                           std::vector<std::unique_ptr<Monster>> &monsters)
{
    weaponManager_.update(deltaTime, playerPos, attackDir, monsters);
}

void Player::addWeapon(std::unique_ptr<Weapon> weapon)
{
    weaponManager_.addWeapon(std::move(weapon));
}

int Player::addXP(int amount)
{
    currentXP_ += amount;
    int levelUps = 0;
    while (currentXP_ >= getXPToNext())
    {
        currentXP_ -= getXPToNext();
        level_++;
        levelUps++;
    }
    return levelUps;
}

float Player::getXPPercent() const
{
    return static_cast<float>(currentXP_) / static_cast<float>(getXPToNext());
}
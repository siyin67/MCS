#include "Monster.h"
#include <cmath>
#include <random>

Monster::Monster()
    : speed_(80.0f), health_(30), maxHealth_(30), damage_(10), expReward_(5), skillCooldown_(0.0f), skillTimer_(0.0f)
{
    shape_.setSize({64, 64});
    shape_.setOrigin({0, 0});
    position_ = {0, 0};
    shape_.setPosition(position_);
}

void Monster::setSkill(std::function<void(Monster &, const sf::Vector2f &)> skill, float cooldown)
{
    // 暂未实现
    (void)skill;
    (void)cooldown;
}

void Monster::updateSkill(float deltaTime, const sf::Vector2f &playerPos)
{
    // 暂未实现
    (void)deltaTime;
    (void)playerPos;
}

void Monster::useSkill(const sf::Vector2f &playerPos)
{
    // 暂未实现
    (void)playerPos;
}
void Monster::update(float deltaTime, const sf::Vector2f &playerPos,
                     const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
    // 1. 朝向玩家
    sf::Vector2f toPlayer = playerPos - position_;
    float len = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    if (len > 0.01f)
    {
        toPlayer.x /= len;
        toPlayer.y /= len;
    }

    // 2. 分离力（远离其他怪物）
    sf::Vector2f separation(0, 0);
    int count = 0;

    for (const auto &other : allMonsters)
    {
        if (other.get() == this)
            continue;

        sf::Vector2f delta = position_ - other->getPosition();
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        if (dist < 60.0f && dist > 0.01f)
        {
            delta.x /= dist;
            delta.y /= dist;
            separation += delta * (1.0f - dist / 60.0f);
            count++;
        }
    }

    if (count > 0)
    {
        separation.x /= count;
        separation.y /= count;
    }

    // 3. 合并方向（玩家吸引力 + 分离力）
    sf::Vector2f move = toPlayer + separation * 0.8f;
    len = std::sqrt(move.x * move.x + move.y * move.y);
    if (len > 0.01f)
    {
        move.x /= len;
        move.y /= len;
    }

    // 4. 移动
    position_ += move * speed_ * deltaTime;
    shape_.setPosition(position_);
}

void Monster::render(sf::RenderWindow &window)
{
    window.draw(shape_);
}

void Monster::takeDamage(int damage)
{
    health_ -= damage;
    if (health_ < 0)
        health_ = 0;
    shape_.setFillColor(sf::Color(255, 100, 100));
}

void Monster::setPosition(const sf::Vector2f &pos)
{
    position_ = pos;
    shape_.setPosition(position_);
}

sf::Vector2f Monster::getPosition() const
{
    return position_;
}

sf::FloatRect Monster::getBounds() const
{
    return shape_.getGlobalBounds();
}

// ICollidable 接口实现
sf::FloatRect Monster::getGlobalBounds() const
{
    return shape_.getGlobalBounds();
}

std::vector<sf::Vector2f> Monster::getVertices() const
{
    return verticesFromBounds(getGlobalBounds());
}

CollisionType Monster::getType() const
{
    return CollisionType::MONSTER;
}
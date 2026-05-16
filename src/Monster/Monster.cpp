#include "Monster.h"
#include <cmath>

Monster::Monster()
    : speed_(80.0f), health_(30), maxHealth_(30), damage_(10), expReward_(5), skillCooldown_(0.0f), skillTimer_(0.0f)
{
    shape_.setSize({64, 64});
    shape_.setOrigin({0, 0});
    position_ = {0, 0};
    shape_.setPosition(position_);
}

void Monster::update(float deltaTime, const sf::Vector2f &playerPos)
{
    // 移动逻辑
    sf::Vector2f direction = playerPos - position_;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.01f)
    {
        direction.x /= length;
        direction.y /= length;
        position_ += direction * speed_ * deltaTime;
        shape_.setPosition(position_);
    }

    // 技能更新
    updateSkill(deltaTime, playerPos);
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
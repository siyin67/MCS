#include "Player.h"
#include <cmath>
#include "Resourcemanager/ResourceManager.h"
#include <iostream>

Player::Player()
    : speed_(500.0f), health_(20), maxHealth_(20)
{
    std::cout << "Player created" << std::endl;
    shape_.setSize({64, 64});
    shape_.setOrigin({32, 32}); // 设置原点为中心
    shape_.setScale({0.8f, 0.8f});

    if (ResourceManager::GetInstance().HasTexture("player"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("player");
        shape_.setTexture(&bgTex);
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

    // 3. 计算新位置
    sf::Vector2f newPos = shape_.getPosition() + movement * speed_ * deltaTime;

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
}

void Player::takeDamage(int damage)
{
    health_ -= damage;
    if (health_ < 0)
        health_ = 0;
    shape_.setFillColor(sf::Color::Red);
}

void Player::heal(int amount)
{
    health_ += amount;
    if (health_ > maxHealth_)
        health_ = maxHealth_;
    shape_.setFillColor(sf::Color::Green);
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
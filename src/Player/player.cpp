#include "Player.h"
#include <cmath>
#include "Resourcemanager/ResourceManager.h"
#include <iostream>

Player::Player()
    : speed_(125.0f), health_(20), maxHealth_(20)
{
    std::cout << "Player created" << std::endl;
    shape_.setSize({64, 64});
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
}

void Player::update(float deltaTime)
{
    sf::Vector2f movement(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        movement.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        movement.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        movement.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        movement.x += 1;

    if (movement.x != 0 || movement.y != 0)
    {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }

    shape_.move(movement * speed_ * deltaTime);
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

void Player::render(sf::RenderWindow &window)
{
    window.draw(shape_);
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
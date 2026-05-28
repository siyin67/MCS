#include "Projectile.h"
#include "Resourcemanager/ResourceManager.h"

Projectile::Projectile()
    : damage_(10), lifetime_(2.0f), maxLifetime_(2.0f), isActive_(true), isFriendly_(false)
{
    shape_.setSize({16, 16});
    shape_.setOrigin({8, 8});
    shape_.setFillColor(sf::Color::White);
    position_ = {0, 0};
    shape_.setPosition(position_);
}

void Projectile::update(float deltaTime)
{
    if (!isActive_)
        return;

    position_ += velocity_ * deltaTime;
    shape_.setPosition(position_);

    lifetime_ -= deltaTime;
    if (lifetime_ <= 0.0f)
    {
        isActive_ = false;
    }
}

void Projectile::render(sf::RenderWindow &window)
{
    if (isActive_)
    {
        window.draw(shape_);
    }
}

void Projectile::setPosition(const sf::Vector2f &pos)
{
    position_ = pos;
    shape_.setPosition(position_);
}

sf::Vector2f Projectile::getPosition() const
{
    return position_;
}

void Projectile::setVelocity(const sf::Vector2f &vel)
{
    velocity_ = vel;
}

sf::Vector2f Projectile::getVelocity() const
{
    return velocity_;
}

void Projectile::setDamage(int damage)
{
    damage_ = damage;
}

int Projectile::getDamage() const
{
    return damage_;
}

void Projectile::setActive(bool active)
{
    isActive_ = active;
}

bool Projectile::isActive() const
{
    return isActive_;
}

void Projectile::setLifetime(float lifetime)
{
    lifetime_ = lifetime;
    maxLifetime_ = lifetime;
}

float Projectile::getLifetime() const
{
    return lifetime_;
}

sf::FloatRect Projectile::getBounds() const
{
    return shape_.getGlobalBounds();
}

void Projectile::setTexture(const sf::Texture &texture)
{
    shape_.setTexture(&texture);
}

void Projectile::setTextureId(const std::string &id)
{
    textureId_ = id;
    if (ResourceManager::GetInstance().HasTexture(id))
    {
        shape_.setTexture(&ResourceManager::GetInstance().GetTexture(id));
    }
}

void Projectile::setSize(const sf::Vector2f &size)
{
    shape_.setSize(size);
    shape_.setOrigin({size.x / 2, size.y / 2});
}

void Projectile::setFillColor(const sf::Color &color)
{
    shape_.setFillColor(color);
}

// ========== ICollidable 接口实现 ==========

sf::FloatRect Projectile::getGlobalBounds() const
{
    return shape_.getGlobalBounds();
}

std::vector<sf::Vector2f> Projectile::getVertices() const
{
    return verticesFromBounds(getGlobalBounds());
}

CollisionType Projectile::getType() const
{
    return CollisionType::BULLET;
}
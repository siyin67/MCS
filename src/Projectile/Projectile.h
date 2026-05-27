#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Collision/Collision.h"

class Monster;

class Projectile : public ICollidable
{
protected:
    sf::RectangleShape shape_;
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    int damage_;
    float lifetime_;
    float maxLifetime_;
    bool isActive_;
    std::string textureId_;

public:
    Projectile();
    virtual ~Projectile() = default;

    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow &window);
    virtual void onHit(Monster &monster) = 0;
    virtual void checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters) { (void)monsters; }
    virtual void setTrackingTarget(const sf::Vector2f &target) { (void)target; }

    // 位置和移动
    void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getPosition() const;
    void setVelocity(const sf::Vector2f &vel);
    sf::Vector2f getVelocity() const;

    // 属性
    void setDamage(int damage);
    int getDamage() const;
    virtual void setActive(bool active);
    bool isActive() const;
    void setLifetime(float lifetime);
    float getLifetime() const;

    // 碰撞箱
    sf::FloatRect getBounds() const;

    // 纹理
    void setTexture(const sf::Texture &texture);
    void setTextureId(const std::string &id);
    const std::string &getTextureId() const { return textureId_; }

    // 形状
    void setSize(const sf::Vector2f &size);
    void setFillColor(const sf::Color &color);

    // ICollidable 接口实现
    sf::FloatRect getGlobalBounds() const override;
    std::vector<sf::Vector2f> getVertices() const override;
    CollisionType getType() const override;
};
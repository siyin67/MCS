#pragma once
#include <SFML/Graphics.hpp>
#include "collision/Collision.h"

class Player : public ICollidable
{
private:
    sf::RectangleShape shape_;
    float speed_;
    int health_;
    int maxHealth_;

    sf::ConvexShape arrow_;
    sf::Vector2f attackDirection_;

public:
    Player();

    void update(float deltaTime);
    void takeDamage(int damage);
    void heal(int amount);

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

    void render(sf::RenderWindow &window);

    int getHealth() const;
    bool isAlive() const;
    float getHealthPercent() const;

    void updateAttackDirection(const sf::RenderWindow &window);
    sf::Vector2f getAttackDirection() const { return attackDirection_; }

    // ICollidable 接口实现
    sf::FloatRect getGlobalBounds() const override;
    std::vector<sf::Vector2f> getVertices() const override;
    CollisionType getType() const override;
};
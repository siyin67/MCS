#pragma once
#include <SFML/Graphics.hpp>
#include "../Collision/Collision.h"

class Player;

class DropItem : public ICollidable
{
protected:
	sf::Vector2f position_;
	sf::RectangleShape shape_;
	float lifetime_;
	float elapsed_;
	bool alive_;
	int value_;

public:
	DropItem(sf::Vector2f pos, int value, float lifetime = 120.0f);
	virtual ~DropItem() = default;

	virtual void update(float deltaTime, const sf::Vector2f &playerPos) = 0;
	virtual void render(sf::RenderWindow &window);
	virtual void onCollect(Player &player) { (void)player; }

	bool isAlive() const { return alive_; }
	int getValue() const { return value_; }
	void collect() { alive_ = false; }
	sf::Vector2f getPosition() const { return position_; }

	// ICollidable 接口
	sf::FloatRect getGlobalBounds() const override;
	std::vector<sf::Vector2f> getVertices() const override;
	CollisionType getType() const override;
};

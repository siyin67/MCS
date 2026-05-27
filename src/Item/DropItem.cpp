#include "DropItem.h"

DropItem::DropItem(sf::Vector2f pos, int value, float lifetime)
	: position_(pos), lifetime_(lifetime), elapsed_(0.0f), alive_(true), value_(value)
{
}

void DropItem::render(sf::RenderWindow &window)
{
	shape_.setPosition(position_);
	window.draw(shape_);
}

sf::FloatRect DropItem::getGlobalBounds() const
{
	return shape_.getGlobalBounds();
}

std::vector<sf::Vector2f> DropItem::getVertices() const
{
	return verticesFromBounds(getGlobalBounds());
}

CollisionType DropItem::getType() const
{
	return CollisionType::DROPITEM;
}
#pragma once
#include "DropItem.h"
#include "Buff/Buff.h"

class PotionItem : public DropItem
{
private:
	BuffType buffType_;
	float buffValue_;
	float buffDuration_;
	float attractTimer_;

public:
	PotionItem(sf::Vector2f pos, BuffType type, float value, float duration);

	void update(float deltaTime, const sf::Vector2f &playerPos) override;
	void render(sf::RenderWindow &window) override;
	void onCollect(Player &player) override;

	BuffType getBuffType() const { return buffType_; }
	float getBuffValue() const { return buffValue_; }
	float getBuffDuration() const { return buffDuration_; }
};

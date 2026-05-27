#pragma once
#include "DropItem.h"
#include <vector>
#include <memory>
#include <functional>

class Player;

class DropItemManager
{
private:
	std::vector<std::unique_ptr<DropItem>> items_;
	int totalXpCollected_ = 0;

public:
	void add(std::unique_ptr<DropItem> item);
	void update(float deltaTime, const sf::Vector2f &playerPos);
	void render(sf::RenderWindow &window);
	void removeDead();

	// 玩家碰撞检测，返回本轮收集的经验值
	int checkPlayerCollision(Player &player);

	int getTotalXpCollected() const { return totalXpCollected_; }
	size_t getCount() const { return items_.size(); }
};
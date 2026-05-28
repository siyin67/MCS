#include "DropItemManager.h"
#include "../Player/player.h"

void DropItemManager::add(std::unique_ptr<DropItem> item)
{
	if (item)
		items_.push_back(std::move(item));
}

void DropItemManager::update(float deltaTime, const sf::Vector2f &playerPos)
{
	for (auto &item : items_)
	{
		if (item->isAlive())
			item->update(deltaTime, playerPos);
	}
}

void DropItemManager::render(sf::RenderWindow &window)
{
	for (auto &item : items_)
	{
		if (item->isAlive())
			item->render(window);
	}
}

void DropItemManager::removeDead()
{
	items_.erase(
		std::remove_if(items_.begin(), items_.end(),
					   [](const std::unique_ptr<DropItem> &item)
					   { return !item->isAlive(); }),
		items_.end());
}

int DropItemManager::checkPlayerCollision(Player &player)
{
	int collected = 0;
	sf::FloatRect playerBounds = player.getGlobalBounds();

	for (auto &item : items_)
	{
		if (!item->isAlive())
			continue;

		if (playerBounds.findIntersection(item->getGlobalBounds()).has_value())
		{
			collected += item->getValue();
			item->onCollect(player);
			item->collect();
		}
	}

	totalXpCollected_ += collected;
	return collected;
}

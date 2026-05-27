#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "Monster.h"

class Spawner
{
private:
	std::vector<std::function<std::unique_ptr<Monster>()>> monsterFactories_;

	int currentWave_;
	int maxAlive_;       // 场上最大怪物数
	float spawnTimer_;
	float spawnInterval_;
	float waveTimer_;
	float waveDuration_;

	sf::Vector2f mapSize_;
	float spawnDistance_;

public:
	Spawner();

	void registerMonster(std::function<std::unique_ptr<Monster>()> factory);
	void setMapSize(const sf::Vector2f &size);
	void setSpawnDistance(float distance);
	void reset();

	void update(float deltaTime, const sf::Vector2f &playerPos,
				std::vector<std::unique_ptr<Monster>> &monsters);

	int getCurrentWave() const { return currentWave_; }

private:
	void startWave();
	sf::Vector2f getSpawnPosition(const sf::Vector2f &playerPos);
	int countAlive(const std::vector<std::unique_ptr<Monster>> &monsters) const;
};

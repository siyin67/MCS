#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "Monster.h"

class Spawner
{
public:
	enum class Tier { Common, Uncommon, Rare, Boss };

private:
	struct SpawnEntry
	{
		int minWave;
		std::function<std::unique_ptr<Monster>()> factory;
	};
	std::vector<SpawnEntry> factories_[4]; // indexed by Tier

	int currentWave_;
	int maxAlive_;
	float spawnTimer_;
	float spawnInterval_;
	float waveTimer_;
	float waveDuration_;

	sf::Vector2f mapSize_;
	float spawnDistance_;

public:
	Spawner();

	void registerMonster(Tier tier, int minWave, std::function<std::unique_ptr<Monster>()> factory);
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

	Tier rollTier() const;
	// 该 Tier 是否有当前波次可用的怪物
	bool tierAvailable(Tier tier) const;
	// 所有 Tier 在当前波次是否都不可用
	bool allTiersUnavailable() const;
};

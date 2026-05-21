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

    // 波次参数
    int currentWave_;
    int monstersToSpawn_;
    int monstersSpawned_;
    float spawnTimer_;
    float spawnInterval_;

    // 时间推进波次
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
    int getRemainingToSpawn() const { return monstersToSpawn_ - monstersSpawned_; }

private:
    void startWave();
    sf::Vector2f getSpawnPosition(const sf::Vector2f &playerPos);
};
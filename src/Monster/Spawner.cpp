#include "Spawner.h"
#include <random>
#include <cmath>
#include <iostream>
#include "Zombie.h"

Spawner::Spawner()
    : currentWave_(1), monstersToSpawn_(100), monstersSpawned_(0), spawnTimer_(0.0f), spawnInterval_(1.5f), waveTimer_(0.0f), waveDuration_(60.0f), mapSize_{5000.0f, 5000.0f}, spawnDistance_(800.0f)
{
    registerMonster([]()
                    { return std::make_unique<Zombie>(); });
}

void Spawner::registerMonster(std::function<std::unique_ptr<Monster>()> factory)
{
    monsterFactories_.push_back(factory);
}

void Spawner::setMapSize(const sf::Vector2f &size)
{
    mapSize_ = size;
}

void Spawner::setSpawnDistance(float distance)
{
    spawnDistance_ = distance;
}

void Spawner::reset()
{
    currentWave_ = 1;
    monstersSpawned_ = 0;
    spawnTimer_ = 0.0f;
    waveTimer_ = 0.0f;
    startWave();
}

void Spawner::startWave()
{
    monstersSpawned_ = 0;
    spawnTimer_ = 0.0f;

    // 怪物数量：5 + 波次/2（最多 50 只）
    monstersToSpawn_ = std::min(50, 5 + currentWave_ / 2);

    // 生成间隔：1.5 秒 → 0.5 秒（最快 0.4 秒）
    spawnInterval_ = std::max(0.4f, 1.5f - currentWave_ / 50.0f);

    // 波次持续时间：60 秒 → 20 秒（最短 20 秒）
    waveDuration_ = std::max(20.0f, 60.0f - currentWave_ / 2.0f);

    std::cout << "=== Wave " << currentWave_ << " ===" << std::endl;
    std::cout << "  Monsters: " << monstersToSpawn_ << std::endl;
    std::cout << "  Spawn interval: " << spawnInterval_ << "s" << std::endl;
    std::cout << "  Duration: " << waveDuration_ << "s" << std::endl;
}

void Spawner::update(float deltaTime, const sf::Vector2f &playerPos,
                     std::vector<std::unique_ptr<Monster>> &monsters)
{
    if (monsterFactories_.empty())
        return;

    // 波次计时推进
    waveTimer_ += deltaTime;
    if (waveTimer_ >= waveDuration_)
    {
        waveTimer_ = 0.0f;
        currentWave_++;
        startWave();
    }

    // 生成怪物
    if (monstersSpawned_ < monstersToSpawn_)
    {
        spawnTimer_ += deltaTime;

        if (spawnTimer_ >= spawnInterval_)
        {
            spawnTimer_ = 0.0f;

            // 随机选择怪物类型
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, monsterFactories_.size() - 1);

            auto monster = monsterFactories_[dis(gen)]();
            monster->setPosition(getSpawnPosition(playerPos));
            monsters.push_back(std::move(monster));
            monstersSpawned_++;
        }
    }
}

sf::Vector2f Spawner::getSpawnPosition(const sf::Vector2f &playerPos)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> angleDis(0, 2 * 3.14159265358979323846f);
    static std::uniform_real_distribution<> distDis(spawnDistance_ - 100, spawnDistance_ + 100);

    float angle = angleDis(gen);
    float distance = distDis(gen);

    sf::Vector2f pos;
    pos.x = playerPos.x + std::cos(angle) * distance;
    pos.y = playerPos.y + std::sin(angle) * distance;

    // 边界限制
    float halfSize = 48.0f;
    pos.x = std::max(halfSize, std::min(pos.x, mapSize_.x - halfSize));
    pos.y = std::max(halfSize, std::min(pos.y, mapSize_.y - halfSize));

    return pos;
}
#include "Spawner.h"
#include <random>
#include <cmath>
#include <iostream>
#include "Zombie.h"
#include "Slimes.h"

Spawner::Spawner()
	: currentWave_(1), maxAlive_(20),
	  spawnTimer_(0.0f), spawnInterval_(1.5f),
	  waveTimer_(0.0f), waveDuration_(30.0f),
	  mapSize_{5000.0f, 5000.0f}, spawnDistance_(800.0f)
{
	// 僵尸（权重最高）
	registerMonster([]() { return std::make_unique<Zombie>(); });
	registerMonster([]() { return std::make_unique<Zombie>(); });
	registerMonster([]() { return std::make_unique<Zombie>(); });
	// 小史莱姆
	registerMonster([]() { return std::make_unique<SmallSlime>(); });
	registerMonster([]() { return std::make_unique<SmallSlime>(); });
	// 大史莱姆（稀有）
	registerMonster([]() { return std::make_unique<BigSlime>(); });
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
	maxAlive_ = 20;
	spawnTimer_ = 0.0f;
	waveTimer_ = 0.0f;
	spawnInterval_ = 1.5f;
}

void Spawner::startWave()
{
	std::cout << "=== Wave " << currentWave_ << " ===" << std::endl;
	std::cout << "  Max alive: " << maxAlive_ << std::endl;
	std::cout << "  Spawn interval: " << spawnInterval_ << "s" << std::endl;
}

void Spawner::update(float deltaTime, const sf::Vector2f &playerPos,
					 std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (monsterFactories_.empty())
		return;

	// 波次推进
	waveTimer_ += deltaTime;
	if (waveTimer_ >= waveDuration_)
	{
		waveTimer_ = 0.0f;
		currentWave_++;
		maxAlive_ = 20 + currentWave_ * 3;
		spawnInterval_ = std::max(0.3f, 1.5f - currentWave_ * 0.05f);
		waveDuration_ = std::max(15.0f, 30.0f - currentWave_ * 0.3f);
		startWave();
	}

	// 持续生成：场上怪物不足时不断补充
	int alive = countAlive(monsters);
	if (alive < maxAlive_)
	{
		spawnTimer_ += deltaTime;
		if (spawnTimer_ >= spawnInterval_)
		{
			spawnTimer_ = 0.0f;

			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, monsterFactories_.size() - 1);

			auto monster = monsterFactories_[dis(gen)]();
			monster->setPosition(getSpawnPosition(playerPos));
			monsters.push_back(std::move(monster));
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

	float halfSize = 48.0f;
	pos.x = std::max(halfSize, std::min(pos.x, mapSize_.x - halfSize));
	pos.y = std::max(halfSize, std::min(pos.y, mapSize_.y - halfSize));

	return pos;
}

int Spawner::countAlive(const std::vector<std::unique_ptr<Monster>> &monsters) const
{
	int count = 0;
	for (const auto &m : monsters)
	{
		if (m->isAlive())
			count++;
	}
	return count;
}

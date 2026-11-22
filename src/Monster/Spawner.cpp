#include "Spawner.h"
#include <random>
#include <cmath>
#include <iostream>
#include "Sound/SoundManager.h"
#include "Zombie.h"
#include "Slimes.h"
#include "Skeleton.h"
#include "Spider.h"
#include "Drowned.h"
#include "Breeze.h"
#include "Creeper.h"
#include "Enderman.h"
#include "Witch.h"
#include "Blaze.h"
#include "WitherSkeleton.h"
#include "Ghast.h"
#include "Wither.h"
#include "Enderdragon.h"

Spawner::Spawner()
	: currentWave_(1), maxAlive_(80),
	  spawnTimer_(0.0f), spawnInterval_(0.8f),
	  waveTimer_(0.0f), waveDuration_(30.0f),
	  mapSize_{5000.0f, 5000.0f}, spawnDistance_(800.0f)
{
	// 常见 Tier（波次要求: 1, 3, 5, 5）
	registerMonster(Tier::Common, 1, []()
					{ return std::make_unique<Zombie>(); });
	registerMonster(Tier::Common, 3, []()
					{ return std::make_unique<Skeleton>(); });
	registerMonster(Tier::Common, 5, []()
					{ return std::make_unique<Spider>(); });
	registerMonster(Tier::Common, 5, []()
					{ return std::make_unique<SmallSlime>(); });
	// 一般 Tier（波次要求: 7, 8, 9, 10, 11, 12, 13）
	registerMonster(Tier::Uncommon, 7, []()
					{ return std::make_unique<Drowned>(); });
	registerMonster(Tier::Uncommon, 8, []()
					{ return std::make_unique<Breeze>(); });
	registerMonster(Tier::Uncommon, 9, []()
					{ return std::make_unique<Creeper>(); });
	registerMonster(Tier::Uncommon, 10, []()
					{ return std::make_unique<Enderman>(); });
	registerMonster(Tier::Uncommon, 11, []()
					{ return std::make_unique<Witch>(); });
	registerMonster(Tier::Uncommon, 12, []()
					{ return std::make_unique<Blaze>(); });
	registerMonster(Tier::Uncommon, 13, []()
					{ return std::make_unique<WitherSkeleton>(); });
	// 稀有 Tier（波次要求: 15, 17）
	registerMonster(Tier::Rare, 15, []()
					{ return std::make_unique<BigSlime>(); });
	registerMonster(Tier::Rare, 17, []()
					{ return std::make_unique<Ghast>(); });
	// Boss Tier（波次要求: 20, 25）
	registerMonster(Tier::Boss, 20, []()
					{ return std::make_unique<Wither>(); });
	registerMonster(Tier::Boss, 25, []()
					{ return std::make_unique<Enderdragon>(); });
}

void Spawner::registerMonster(Tier tier, int minWave, std::function<std::unique_ptr<Monster>()> factory)
{
	factories_[(int)tier].push_back({minWave, factory});
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
	maxAlive_ = 80;
	spawnTimer_ = 0.0f;
	waveTimer_ = 0.0f;
	spawnInterval_ = 0.4f;
}

void Spawner::startWave()
{
	std::cout << "=== Wave " << currentWave_ << " ===" << std::endl;
	std::cout << "  Max alive: " << maxAlive_ << std::endl;
	std::cout << "  Spawn interval: " << spawnInterval_ << "s" << std::endl;
}

bool Spawner::tierAvailable(Tier tier) const
{
	for (const auto &entry : factories_[(int)tier])
	{
		if (entry.minWave <= currentWave_)
			return true;
	}
	return false;
}

bool Spawner::allTiersUnavailable() const
{
	for (int i = 0; i < 4; ++i)
		if (tierAvailable((Tier)i))
			return false;
	return true;
}

Spawner::Tier Spawner::rollTier() const
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> dis(0.0, 100.0);

	double roll = dis(gen);

	double commonPct, uncommonPct, rarePct, bossPct;
	if (currentWave_ <= 3)
	{
		commonPct = 80.0;
		uncommonPct = 20.0;
		rarePct = 0.0;
		bossPct = 0.0;
	}
	else if (currentWave_ <= 6)
	{
		commonPct = 70.0;
		uncommonPct = 28.0;
		rarePct = 2.0;
		bossPct = 0.0;
	}
	else if (currentWave_ <= 9)
	{
		commonPct = 60.0;
		uncommonPct = 35.0;
		rarePct = 5.0;
		bossPct = 0.0;
	}
	else if (currentWave_ <= 12)
	{
		commonPct = 50.0;
		uncommonPct = 40.0;
		rarePct = 9.0;
		bossPct = 0.5;
	}
	else if (currentWave_ <= 15)
	{
		commonPct = 40.0;
		uncommonPct = 45.0;
		rarePct = 14.0;
		bossPct = 1.0;
	}
	else if (currentWave_ <= 18)
	{
		commonPct = 30.0;
		uncommonPct = 45.0;
		rarePct = 23.0;
		bossPct = 1.5;
	}
	else
	{
		commonPct = 20.0;
		uncommonPct = 45.0;
		rarePct = 30.0;
		bossPct = 2.0;
	}

	// 按概率区间选择 Tier，若该 Tier 当前波次无可用怪物则跳过
	struct TierProb
	{
		double pct;
		Tier tier;
	};
	TierProb tiers[] = {
		{bossPct, Tier::Boss},
		{rarePct, Tier::Rare},
		{uncommonPct, Tier::Uncommon},
		{commonPct, Tier::Common},
	};

	double cumulative = 0.0;
	for (auto &tp : tiers)
	{
		if (tp.pct <= 0.0)
			continue;
		cumulative += tp.pct;
		if (roll <= cumulative)
		{
			if (tierAvailable(tp.tier))
				return tp.tier;
		}
	}

	// 回退：从 Boss 向下找第一个当前波次可用的 Tier
	for (int i = (int)Tier::Boss; i >= (int)Tier::Common; --i)
	{
		if (tierAvailable((Tier)i))
			return (Tier)i;
	}

	return Tier::Common; // unreachable if at least one factory is registered at wave 1
}

void Spawner::update(float deltaTime, const sf::Vector2f &playerPos,
					 std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (allTiersUnavailable())
		return;

	waveTimer_ += deltaTime;
	if (waveTimer_ >= waveDuration_)
	{
		waveTimer_ = 0.0f;
		currentWave_++;
		maxAlive_ = 80 + currentWave_ * 12;
		spawnInterval_ = std::max(0.1f, 0.4f - currentWave_ * 0.02f);
		waveDuration_ = std::max(15.0f, 30.0f - currentWave_ * 0.3f);
		startWave();
	}

	int alive = countAlive(monsters);
	if (alive < maxAlive_)
	{
		spawnTimer_ += deltaTime;
		if (spawnTimer_ >= spawnInterval_)
		{
			spawnTimer_ = 0.0f;

			Tier tier = rollTier();
			auto &pool = factories_[(int)tier];

			// 收集当前波次可用的工厂
			static std::vector<std::function<std::unique_ptr<Monster>()>> available;
			available.clear();
			for (const auto &entry : pool)
			{
				if (entry.minWave <= currentWave_)
					available.push_back(entry.factory);
			}

			if (available.empty())
				return;

			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, available.size() - 1);

			auto monster = available[dis(gen)]();
			monster->setPosition(getSpawnPosition(playerPos));

			// 按层级和波次增加血量
			int hpBonus = 0;
			switch (tier)
			{
			case Tier::Common:
				hpBonus = 2 * currentWave_;
				break;
			case Tier::Uncommon:
				hpBonus = 3 * currentWave_;
				break;
			case Tier::Rare:
				hpBonus = 5 * currentWave_;
				break;
			case Tier::Boss:
				hpBonus = 20 * currentWave_;
				SoundManager::getInstance().playSFX("boss_spawn");
				break;
			}
			monster->addHealth(hpBonus);

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

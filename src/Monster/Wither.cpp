#include "Wither.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>
#include <random>

Wither::Wither()
{
	auto cfg = loadJsonConfig("assets/config/monster/wither.json");
	speed_ = cfg.value("speed", 50.0f);
	health_ = cfg.value("health", 120);
	maxHealth_ = cfg.value("maxHealth", 120);
	damage_ = cfg.value("damage", 5);
	expReward_ = cfg.value("expReward", 50);
	std::string texId = cfg.value("textureId", "wither");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
	projSpeed_ = cfg.value("projectileSpeed", 200.0f);
	pattern_ = AttackPattern::BurstFan;
	patternTimer_ = 0.0f;
	fireInterval_ = 0.0f;
}
void Wither::playHitSound()
{
	SoundManager::getInstance().playSFX("wither_hurt");
}
void Wither::playDeathSound()
{
	SoundManager::getInstance().playSFX("wither_death");
}

void Wither::fireFromAngle(const sf::Vector2f &dir)
{
	auto skull = std::make_unique<MonsterProjectile>();
	skull->setPosition(position_);
	skull->setDirection(dir);
	skull->setSpeed(projSpeed_);
	skull->setDamage(damage_);
	skull->setLifetime(3.0f);
	skull->setTextureId("wither_skull");
	skull->setSize({24, 24});
	skull->setAreaParams(BuffType::Wither, 100.0f, 4.0f, 3);
	skull->setOnSpawnArea([](sf::Vector2f pos, float r, float dur, BuffType t, int v)
	{
		if (Monster::spawnAreaCallback)
			Monster::spawnAreaCallback(pos, r, dur, t, v);
	});
	Monster::pendingProjectiles.push_back(std::move(skull));
}

void Wither::update(float deltaTime, const sf::Vector2f &playerPos,
					const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
	Monster::update(deltaTime, playerPos, allMonsters);

	patternTimer_ += deltaTime;

	float phaseDur = 0.0f;
	switch (pattern_)
	{
	case AttackPattern::BurstFan:  phaseDur = BURST_DURATION; break;
	case AttackPattern::Circle:    phaseDur = CIRCLE_DURATION; break;
	case AttackPattern::RapidSpray: phaseDur = SPRAY_DURATION; break;
	}

	if (patternTimer_ >= phaseDur)
	{
		patternTimer_ = 0.0f;
		fireInterval_ = 0.0f;
		switch (pattern_)
		{
		case AttackPattern::BurstFan:  pattern_ = AttackPattern::Circle; break;
		case AttackPattern::Circle:    pattern_ = AttackPattern::RapidSpray; break;
		case AttackPattern::RapidSpray: pattern_ = AttackPattern::BurstFan; break;
		}
	}

	sf::Vector2f baseDir = playerPos - position_;
	float len = std::sqrt(baseDir.x * baseDir.x + baseDir.y * baseDir.y);
	if (len < 0.01f) baseDir = {1.0f, 0.0f};
	else baseDir /= len;

	switch (pattern_)
	{
	case AttackPattern::BurstFan:
		fireInterval_ += deltaTime;
		if (fireInterval_ >= 1.0f)
		{
			fireInterval_ = 0.0f;
			for (int i = -1; i <= 1; ++i)
			{
				float angle = i * 0.2f;
				float cosA = std::cos(angle), sinA = std::sin(angle);
				fireFromAngle({baseDir.x * cosA - baseDir.y * sinA,
							   baseDir.x * sinA + baseDir.y * cosA});
			}
		}
		break;
	case AttackPattern::Circle:
		fireInterval_ += deltaTime;
		if (fireInterval_ >= 0.6f)
		{
			fireInterval_ = 0.0f;
			for (int i = 0; i < 12; ++i)
			{
				float angle = i * 3.14159265358979323846f * 2.0f / 12.0f;
				fireFromAngle({std::cos(angle), std::sin(angle)});
			}
		}
		break;
	case AttackPattern::RapidSpray:
		fireInterval_ += deltaTime;
		if (fireInterval_ >= 0.16f)
		{
			fireInterval_ = 0.0f;
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_real_distribution<> angleDis(0.0, 6.283185307179586);
			for (int i = 0; i < 7; ++i)
			{
				float angle = angleDis(gen);
				fireFromAngle({std::cos(angle), std::sin(angle)});
			}
		}
		break;
	}
}

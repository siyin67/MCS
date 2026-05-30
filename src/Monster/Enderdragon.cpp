#include "Enderdragon.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>
#include <random>

Enderdragon::Enderdragon()
{
	auto cfg = loadJsonConfig("assets/config/monster/enderdragon.json");
	speed_ = cfg.value("speed", 70.0f);
	health_ = cfg.value("health", 200);
	maxHealth_ = cfg.value("maxHealth", 200);
	damage_ = cfg.value("damage", 8);
	expReward_ = cfg.value("expReward", 100);
	std::string texId = cfg.value("textureId", "enderdragon");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});
	projSpeed_ = cfg.value("projectileSpeed", 250.0f);
	projLifetime_ = cfg.value("projLifetime", 3.0f);
	projSize_ = cfg.value("projSize", 28);
	areaChance_ = cfg.value("areaChance", 1.0f);
	areaRadius_ = cfg.value("areaRadius", 100.0f);
	areaDuration_ = cfg.value("areaDuration", 5.0f);
	areaDamage_ = cfg.value("areaDamage", 4);
	burstAngle_ = cfg.value("burstAngle", 0.15f);
	burstCount_ = cfg.value("burstCount", 5);
	burstInterval_ = cfg.value("burstInterval", 1.0f);
	burstDuration_ = cfg.value("burstDuration", 10.0f);
	circleCount_ = cfg.value("circleCount", 16);
	circleInterval_ = cfg.value("circleInterval", 0.6f);
	circleDuration_ = cfg.value("circleDuration", 6.0f);
	sprayCount_ = cfg.value("sprayCount", 9);
	sprayInterval_ = cfg.value("sprayInterval", 0.12f);
	sprayDuration_ = cfg.value("sprayDuration", 4.0f);
	laserCooldown_ = cfg.value("laserCooldown", 30.0f);
	laserChargeTime_ = cfg.value("laserChargeTime", 3.0f);
	laserFireTime_ = cfg.value("laserFireTime", 1.0f);
	laserTimer_ = 0.0f;
	laserChargeTimer_ = 0.0f;
	laserCharging_ = false;
	laserFiring_ = false;
	laserLineLength_ = 2000.0f;
	pattern_ = AttackPattern::BurstFan;
	patternTimer_ = 0.0f;
	fireInterval_ = 0.0f;
}
void Enderdragon::playHitSound()
{
	SoundManager::getInstance().playSFX("enderdragon_hurt");
}
void Enderdragon::playDeathSound()
{
	SoundManager::getInstance().playSFX("enderdragon_death");
}

void Enderdragon::fireFromAngle(const sf::Vector2f &dir)
{
	auto dball = std::make_unique<MonsterProjectile>();
	dball->setPosition(position_);
	dball->setDirection(dir);
	dball->setSpeed(projSpeed_);
	dball->setDamage(damage_);
	dball->setLifetime(projLifetime_);
	dball->setTextureId("dragon_fireball");
	dball->setSize({(float)projSize_, (float)projSize_});
	dball->setAreaChance(areaChance_);
	dball->setAreaParams(BuffType::Poison, areaRadius_, areaDuration_, areaDamage_);
	dball->setOnSpawnArea([](sf::Vector2f pos, float r, float dur, BuffType t, int v)
						  {
		if (Monster::spawnAreaCallback)
			Monster::spawnAreaCallback(pos, r, dur, t, v); });
	Monster::pendingProjectiles.push_back(std::move(dball));
}

void Enderdragon::update(float deltaTime, const sf::Vector2f &playerPos,
						 const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
	Monster::update(deltaTime, playerPos, allMonsters);

	patternTimer_ += deltaTime;
	float phaseDur = 0.0f;
	switch (pattern_)
	{
	case AttackPattern::BurstFan:
		phaseDur = burstDuration_;
		break;
	case AttackPattern::Circle:
		phaseDur = circleDuration_;
		break;
	case AttackPattern::RapidSpray:
		phaseDur = sprayDuration_;
		break;
	}

	if (patternTimer_ >= phaseDur)
	{
		patternTimer_ = 0.0f;
		fireInterval_ = 0.0f;
		switch (pattern_)
		{
		case AttackPattern::BurstFan:
			pattern_ = AttackPattern::Circle;
			break;
		case AttackPattern::Circle:
			pattern_ = AttackPattern::RapidSpray;
			break;
		case AttackPattern::RapidSpray:
			pattern_ = AttackPattern::BurstFan;
			break;
		}
	}

	sf::Vector2f baseDir = playerPos - position_;
	float len = std::sqrt(baseDir.x * baseDir.x + baseDir.y * baseDir.y);
	if (len < 0.01f)
		baseDir = {1.0f, 0.0f};
	else
		baseDir /= len;

	switch (pattern_)
	{
	case AttackPattern::BurstFan:
		fireInterval_ += deltaTime;
		if (fireInterval_ >= burstInterval_)
		{
			fireInterval_ = 0.0f;
			for (int i = -(burstCount_/2); i <= burstCount_/2; ++i)
			{
				float angle = i * burstAngle_;
				float cosA = std::cos(angle), sinA = std::sin(angle);
				fireFromAngle({baseDir.x * cosA - baseDir.y * sinA,
							   baseDir.x * sinA + baseDir.y * cosA});
			}
		}
		break;
	case AttackPattern::Circle:
		fireInterval_ += deltaTime;
		if (fireInterval_ >= circleInterval_)
		{
			fireInterval_ = 0.0f;
			for (int i = 0; i < circleCount_; ++i)
			{
				float angle = i * 3.14159265358979323846f * 2.0f / 16.0f;
				fireFromAngle({std::cos(angle), std::sin(angle)});
			}
		}
		break;
	case AttackPattern::RapidSpray:
		fireInterval_ += deltaTime;
		if (fireInterval_ >= sprayInterval_)
		{
			fireInterval_ = 0.0f;
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_real_distribution<> angleDis(0.0, 6.283185307179586);
			for (int i = 0; i < sprayCount_; ++i)
			{
				float angle = angleDis(gen);
				fireFromAngle({std::cos(angle), std::sin(angle)});
			}
		}
		break;
	}

	laserTimer_ += deltaTime;

	if (laserCharging_)
	{
		laserChargeTimer_ -= deltaTime;
		if (laserChargeTimer_ <= 0.0f)
		{
			laserCharging_ = false;
			laserFiring_ = true;
			laserChargeTimer_ = laserFireTime_;
			SoundManager::getInstance().playSFX("enderdragon_laser");
		}
	}
	else if (laserFiring_)
	{
		laserChargeTimer_ -= deltaTime;
		sf::Vector2f toPlayer = playerPos - position_;
		float projDist = toPlayer.x * laserDir_.x + toPlayer.y * laserDir_.y;
		float perpDist = std::abs(-laserDir_.y * toPlayer.x + laserDir_.x * toPlayer.y);
		float beamHalfWidth = 5.0f * 64.0f / 2.0f;
		if (std::abs(projDist) < laserLineLength_ && perpDist < beamHalfWidth)
		{
			if (Monster::spawnAreaCallback)
				Monster::spawnAreaCallback(playerPos, 20.0f, 0.2f, BuffType::Burn, damage_ * 3);
		}
		if (laserChargeTimer_ <= 0.0f)
		{
			laserFiring_ = false;
			laserTimer_ = 0.0f;
		}
	}
	else if (laserTimer_ >= laserCooldown_)
	{
		laserCharging_ = true;
		laserChargeTimer_ = laserChargeTime_;
		sf::Vector2f dir = playerPos - position_;
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		laserDir_ = len > 0.01f ? dir / len : sf::Vector2f(1.0f, 0.0f);
	}
}

void Enderdragon::render(sf::RenderWindow &window)
{
	Monster::render(window);

	if (!laserCharging_ && !laserFiring_)
		return;

	sf::Vector2f lineStart = position_ - laserDir_ * laserLineLength_;
	sf::Vector2f lineEnd = position_ + laserDir_ * laserLineLength_;

	if (laserCharging_)
	{
		sf::Vertex line[] = {
			{lineStart, sf::Color::Red},
			{lineEnd, sf::Color::Red}};
		window.draw(line, 2, sf::PrimitiveType::Lines);
	}
	else if (laserFiring_)
	{
		float beamHalfWidth = 5.0f * 64.0f / 2.0f;
		sf::Vector2f perp(-laserDir_.y * beamHalfWidth, laserDir_.x * beamHalfWidth);

		sf::VertexArray beam(sf::PrimitiveType::TriangleStrip, 4);
		beam[0].position = lineStart - perp;
		beam[0].color = sf::Color(180, 50, 220, 200);
		beam[1].position = lineStart + perp;
		beam[1].color = sf::Color(180, 50, 220, 200);
		beam[2].position = lineEnd - perp;
		beam[2].color = sf::Color(100, 20, 140, 100);
		beam[3].position = lineEnd + perp;
		beam[3].color = sf::Color(100, 20, 140, 100);
		window.draw(beam);
	}
}

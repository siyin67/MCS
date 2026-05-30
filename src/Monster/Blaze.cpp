#include "Blaze.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Blaze::Blaze()
{
	auto cfg = loadJsonConfig("assets/config/monster/blaze.json");
	speed_ = cfg.value("speed", 70.0f);
	health_ = cfg.value("health", 30);
	maxHealth_ = cfg.value("maxHealth", 30);
	damage_ = cfg.value("damage", 2);
	expReward_ = cfg.value("expReward", 8);
	std::string texId = cfg.value("textureId", "blaze");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});

	float skillCD = cfg.value("skillCooldown", 12.0f);
	float projSpeed = cfg.value("projectileSpeed", 400.0f);
	int fireballCount = cfg.value("fireballCount", 6);
	float lifetime = cfg.value("projLifetime", 2.5f);
	int projSize = cfg.value("projSize", 22);
	float spawnOffset = cfg.value("spawnOffset", 24.0f);
	setSkill([projSpeed, fireballCount, lifetime, projSize, spawnOffset](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len < 0.01f) return;
		dir /= len;

		sf::Vector2f spawnPos = self.getPosition();
		for (int i = 0; i < fireballCount; ++i)
		{
			auto fireball = std::make_unique<MonsterProjectile>();
			fireball->setPosition(spawnPos);
			fireball->setDirection(dir);
			fireball->setSpeed(projSpeed);
			fireball->setDamage(self.getDamage());
			fireball->setLifetime(lifetime);
			fireball->setTextureId("fireball");
			fireball->setSize({(float)projSize, (float)projSize});
			Monster::pendingProjectiles.push_back(std::move(fireball));
			spawnPos += dir * spawnOffset;
		}
		SoundManager::getInstance().playSFX("blaze_shoot");
	}, skillCD);
}
void Blaze::playHitSound()
{
	SoundManager::getInstance().playSFX("blaze_hurt");
}
void Blaze::playDeathSound()
{
	SoundManager::getInstance().playSFX("blaze_death");
}

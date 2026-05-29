#include "Ghast.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Ghast::Ghast()
{
	auto cfg = loadJsonConfig("assets/config/monster/ghast.json");
	speed_ = cfg.value("speed", 60.0f);
	health_ = cfg.value("health", 60);
	maxHealth_ = cfg.value("maxHealth", 60);
	damage_ = cfg.value("damage", 3);
	expReward_ = cfg.value("expReward", 20);
	std::string texId = cfg.value("textureId", "ghast");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});

	float skillCD = cfg.value("skillCooldown", 20.0f);
	float projSpeed = cfg.value("projectileSpeed", 250.0f);
	setSkill([projSpeed](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len < 0.01f) return;
		dir /= len;

		auto fireball = std::make_unique<MonsterProjectile>();
		fireball->setPosition(self.getPosition());
		fireball->setDirection(dir);
		fireball->setSpeed(projSpeed);
		fireball->setDamage(self.getDamage());
		fireball->setLifetime(4.0f);
		fireball->setTextureId("fireball");
		fireball->setSize({32, 32});
		fireball->setAreaParams(BuffType::Burn, 80.0f, 0.3f, 6);
		fireball->setOnSpawnArea([](sf::Vector2f pos, float r, float dur, BuffType t, int v)
		{
			if (Monster::spawnAreaCallback)
				Monster::spawnAreaCallback(pos, r, dur, t, v);
		});
		Monster::pendingProjectiles.push_back(std::move(fireball));
		SoundManager::getInstance().playSFX("ghast_shoot");
	}, skillCD);
}
void Ghast::playHitSound()
{
	SoundManager::getInstance().playSFX("ghast_hurt");
}
void Ghast::playDeathSound()
{
	SoundManager::getInstance().playSFX("ghast_death");
}

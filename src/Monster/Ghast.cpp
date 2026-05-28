#include "Ghast.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Ghast::Ghast()
{
	speed_ = 60.0f;
	health_ = 60;
	maxHealth_ = 60;
	damage_ = 3;
	expReward_ = 20;
	if (ResourceManager::GetInstance().HasTexture("ghast"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("ghast");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});

	setSkill([](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len < 0.01f) return;
		dir /= len;

		auto fireball = std::make_unique<MonsterProjectile>();
		fireball->setPosition(self.getPosition());
		fireball->setDirection(dir);
		fireball->setSpeed(250.0f);
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
	}, 20.0f);
}
void Ghast::playHitSound()
{
	SoundManager::getInstance().playSFX("ghast_hurt");
}
void Ghast::playDeathSound()
{
	SoundManager::getInstance().playSFX("ghast_death");
}

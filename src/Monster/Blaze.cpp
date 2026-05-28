#include "Blaze.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Blaze::Blaze()
{
	speed_ = 70.0f;
	health_ = 30;
	maxHealth_ = 30;
	damage_ = 2;
	expReward_ = 8;
	if (ResourceManager::GetInstance().HasTexture("blaze"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("blaze");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});

	setSkill([](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len < 0.01f) return;
		dir /= len;

		// 机枪式连射：同一方向发射一串火球
		sf::Vector2f spawnPos = self.getPosition();
		for (int i = 0; i < 6; ++i)
		{
			auto fireball = std::make_unique<MonsterProjectile>();
			fireball->setPosition(spawnPos);
			fireball->setDirection(dir);
			fireball->setSpeed(400.0f);
			fireball->setDamage(self.getDamage());
			fireball->setLifetime(2.5f);
			fireball->setTextureId("fireball");
			fireball->setSize({22, 22});
			Monster::pendingProjectiles.push_back(std::move(fireball));

			spawnPos += dir * 24.0f; // 沿发射方向错开位置，形成一溜
		}
	}, 6.0f);
}
void Blaze::playHitSound()
{
	SoundManager::getInstance().playSFX("blaze_hurt");
}
void Blaze::playDeathSound()
{
	SoundManager::getInstance().playSFX("blaze_death");
}

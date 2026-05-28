#include "Spider.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Spider::Spider()
{
	speed_ = 150.0f;
	health_ = 20;
	maxHealth_ = 20;
	damage_ = 1;
	expReward_ = 3;
	if (ResourceManager::GetInstance().HasTexture("spider"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("spider");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});

	setSkill([](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len > 0.01f) { dir.x /= len; dir.y /= len; }
		auto web = std::make_unique<MonsterProjectile>();
		web->setPosition(self.getPosition());
		web->setDirection(dir);
		web->setSpeed(250.0f);
		web->setDamage(self.getDamage());
		web->setLifetime(2.0f);
		web->setTextureId("web");
		web->setSize({24, 24});
		web->setHitBuff(BuffType::SpeedDown, 0.7f, 3.0f);
		Monster::pendingProjectiles.push_back(std::move(web));
		SoundManager::getInstance().playSFX("spider_shoot");
	}, 10.0f);
}
void Spider::playHitSound()
{
	SoundManager::getInstance().playSFX("spider_hurt");
}
void Spider::playDeathSound()
{
	SoundManager::getInstance().playSFX("spider_death");
}

#include "Skeleton.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Projectile/Arrow.h"
#include <cmath>

Skeleton::Skeleton()
{
	speed_ = 100.0f;
	health_ = 25;
	maxHealth_ = 25;
	damage_ = 1;
	expReward_ = 4;
	if (ResourceManager::GetInstance().HasTexture("skeleton"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("skeleton");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});

	setSkill([](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len > 0.01f) { dir.x /= len; dir.y /= len; }
		auto arrow = std::make_unique<Arrow>();
		arrow->setPosition(self.getPosition());
		arrow->setVelocity(dir * 350.0f);
		arrow->setDamage(self.getDamage());
		arrow->setLifetime(3.0f);
		Monster::pendingProjectiles.push_back(std::move(arrow));
	}, 5.0f);
}
void Skeleton::playHitSound()
{
	SoundManager::getInstance().playSFX("skeleton_hurt");
}
void Skeleton::playDeathSound()
{
	SoundManager::getInstance().playSFX("skeleton_death");
}

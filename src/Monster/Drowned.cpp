#include "Drowned.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Projectile/TridentProjectile.h"
#include <cmath>

Drowned::Drowned()
{
	speed_ = 85.0f;
	health_ = 35;
	maxHealth_ = 35;
	damage_ = 1;
	expReward_ = 6;
	if (ResourceManager::GetInstance().HasTexture("drowned"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("drowned");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});

	setSkill([](Monster &self, const sf::Vector2f &playerPos)
	{
		sf::Vector2f dir = playerPos - self.getPosition();
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len > 0.01f) { dir.x /= len; dir.y /= len; }
		auto trident = std::make_unique<TridentProjectile>(
			self.getPosition(), dir, self.getDamage(), 600.0f, 350.0f);
		Monster::pendingProjectiles.push_back(std::move(trident));
	}, 5.0f);
}
void Drowned::playHitSound()
{
	SoundManager::getInstance().playSFX("drowned_hurt");
}
void Drowned::playDeathSound()
{
	SoundManager::getInstance().playSFX("drowned_death");
}

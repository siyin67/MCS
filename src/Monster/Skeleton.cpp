#include "Skeleton.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

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
}
void Skeleton::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Skeleton::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

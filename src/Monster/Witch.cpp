#include "Witch.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Witch::Witch()
{
	speed_ = 90.0f;
	health_ = 35;
	maxHealth_ = 35;
	damage_ = 0;
	expReward_ = 8;
	if (ResourceManager::GetInstance().HasTexture("witch"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("witch");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Witch::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Witch::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

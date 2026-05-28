#include "Drowned.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

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
}
void Drowned::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Drowned::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

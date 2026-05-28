#include "Creeper.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Creeper::Creeper()
{
	speed_ = 90.0f;
	health_ = 30;
	maxHealth_ = 30;
	damage_ = 0;
	expReward_ = 8;
	if (ResourceManager::GetInstance().HasTexture("creeper"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("creeper");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Creeper::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Creeper::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

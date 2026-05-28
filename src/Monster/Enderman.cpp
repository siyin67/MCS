#include "Enderman.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Enderman::Enderman()
{
	speed_ = 80.0f;
	health_ = 40;
	maxHealth_ = 40;
	damage_ = 2;
	expReward_ = 10;
	if (ResourceManager::GetInstance().HasTexture("enderman"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("enderman");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Enderman::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Enderman::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

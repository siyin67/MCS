#include "Breeze.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Breeze::Breeze()
{
	speed_ = 110.0f;
	health_ = 28;
	maxHealth_ = 28;
	damage_ = 1;
	expReward_ = 5;
	if (ResourceManager::GetInstance().HasTexture("breeze"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("breeze");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Breeze::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Breeze::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

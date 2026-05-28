#include "Blaze.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

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
}
void Blaze::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Blaze::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

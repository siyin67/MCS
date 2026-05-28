#include "Wither.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Wither::Wither()
{
	speed_ = 50.0f;
	health_ = 120;
	maxHealth_ = 120;
	damage_ = 5;
	expReward_ = 50;
	if (ResourceManager::GetInstance().HasTexture("wither"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("wither");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Wither::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Wither::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

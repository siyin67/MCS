#include "Ghast.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Ghast::Ghast()
{
	speed_ = 60.0f;
	health_ = 60;
	maxHealth_ = 60;
	damage_ = 3;
	expReward_ = 20;
	if (ResourceManager::GetInstance().HasTexture("ghast"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("ghast");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Ghast::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Ghast::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

#include "Spider.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Spider::Spider()
{
	speed_ = 150.0f;
	health_ = 20;
	maxHealth_ = 20;
	damage_ = 1;
	expReward_ = 3;
	if (ResourceManager::GetInstance().HasTexture("spider"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("spider");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Spider::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Spider::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

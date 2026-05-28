#include "Enderdragon.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

Enderdragon::Enderdragon()
{
	speed_ = 70.0f;
	health_ = 200;
	maxHealth_ = 200;
	damage_ = 8;
	expReward_ = 100;
	if (ResourceManager::GetInstance().HasTexture("enderdragon"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("enderdragon");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Enderdragon::playHitSound()
{
	SoundManager::getInstance().playSFX("zombie_hurt");
}
void Enderdragon::playDeathSound()
{
	SoundManager::getInstance().playSFX("zombie_death");
}

#include "WitherSkeleton.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"

WitherSkeleton::WitherSkeleton()
{
	speed_ = 100.0f;
	health_ = 45;
	maxHealth_ = 45;
	damage_ = 2;
	expReward_ = 10;
	if (ResourceManager::GetInstance().HasTexture("wither_skeleton"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("wither_skeleton");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void WitherSkeleton::playHitSound()
{
	SoundManager::getInstance().playSFX("skeleton_hurt");
}
void WitherSkeleton::playDeathSound()
{
	SoundManager::getInstance().playSFX("skeleton_death");
}

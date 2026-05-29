#include "WitherSkeleton.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"

WitherSkeleton::WitherSkeleton()
{
	auto cfg = loadJsonConfig("assets/config/monster/wither_skeleton.json");
	speed_ = cfg.value("speed", 100.0f);
	health_ = cfg.value("health", 45);
	maxHealth_ = cfg.value("maxHealth", 45);
	damage_ = cfg.value("damage", 2);
	expReward_ = cfg.value("expReward", 10);
	std::string texId = cfg.value("textureId", "wither_skeleton");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
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

#include "Breeze.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>

Breeze::Breeze()
{
	auto cfg = loadJsonConfig("assets/config/monster/breeze.json");
	speed_ = cfg.value("speed", 110.0f);
	health_ = cfg.value("health", 28);
	maxHealth_ = cfg.value("maxHealth", 28);
	damage_ = cfg.value("damage", 1);
	expReward_ = cfg.value("expReward", 5);
	std::string texId = cfg.value("textureId", "breeze");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});
	vortexRange_ = cfg.value("vortexRange", 200.0f);
	vortexStrength_ = cfg.value("vortexStrength", 300.0f);
}
void Breeze::playHitSound()
{
	SoundManager::getInstance().playSFX("breeze_hurt");
}
void Breeze::playDeathSound()
{
	SoundManager::getInstance().playSFX("breeze_death");
}
void Breeze::update(float deltaTime, const sf::Vector2f &playerPos,
					 const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
	Monster::update(deltaTime, playerPos, allMonsters);

	sf::Vector2f toPlayer = playerPos - position_;
	float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

	if (dist < vortexRange_ && dist > 10.0f)
	{
		float strength = (1.0f - dist / vortexRange_) * vortexStrength_;
		sf::Vector2f pullDir = toPlayer / dist;
		Monster::pendingPlayerForce -= pullDir * strength;

		static float soundTimer = 0.0f;
		soundTimer += deltaTime;
		if (soundTimer >= 2.0f)
		{
			soundTimer = 0.0f;
			SoundManager::getInstance().playSFX("breeze_vortex");
		}
	}
}

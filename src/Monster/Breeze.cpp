#include "Breeze.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <cmath>

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
	float vortexRange = 200.0f;

	if (dist < vortexRange && dist > 10.0f)
	{
		float strength = (1.0f - dist / vortexRange) * 300.0f;
		sf::Vector2f pullDir = toPlayer / dist;
		Monster::pendingPlayerForce -= pullDir * strength;

		// 涡流音效（低频率播放）
		static float soundTimer = 0.0f;
		soundTimer += deltaTime;
		if (soundTimer >= 2.0f)
		{
			soundTimer = 0.0f;
			SoundManager::getInstance().playSFX("breeze_vortex");
		}
	}
}

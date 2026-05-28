#include "Enderman.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <cmath>
#include <random>

Enderman::Enderman()
{
	speed_ = 80.0f;
	health_ = 40;
	maxHealth_ = 40;
	damage_ = 2;
	expReward_ = 10;
	teleportCooldown_ = 0.0f;
	if (ResourceManager::GetInstance().HasTexture("enderman"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("enderman");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
}
void Enderman::playHitSound()
{
	SoundManager::getInstance().playSFX("enderman_hurt");
}
void Enderman::playDeathSound()
{
	SoundManager::getInstance().playSFX("enderman_death");
}
void Enderman::update(float deltaTime, const sf::Vector2f &playerPos,
					   const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
	Monster::update(deltaTime, playerPos, allMonsters);

	teleportCooldown_ -= deltaTime;
	if (teleportCooldown_ > 0.0f)
		return;

	sf::Vector2f toPlayer = playerPos - position_;
	float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
	float teleportDist = 20.0f * 64.0f;

	if (dist > teleportDist)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<> angleDis(0.0f, 2.0f * 3.14159265358979323846f);
		std::uniform_real_distribution<> distDis(150.0f, 250.0f);

		float angle = angleDis(gen);
		float offset = distDis(gen);
		position_ = playerPos + sf::Vector2f(std::cos(angle), std::sin(angle)) * offset;
		shape_.setPosition(position_);
		teleportCooldown_ = 3.0f;
	}
}

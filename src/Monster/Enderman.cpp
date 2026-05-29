#include "Enderman.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>
#include <random>

Enderman::Enderman()
{
	auto cfg = loadJsonConfig("assets/config/monster/enderman.json");
	speed_ = cfg.value("speed", 80.0f);
	health_ = cfg.value("health", 40);
	maxHealth_ = cfg.value("maxHealth", 40);
	damage_ = cfg.value("damage", 2);
	expReward_ = cfg.value("expReward", 10);
	std::string texId = cfg.value("textureId", "enderman");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});
	teleportCooldown_ = 0.0f;
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

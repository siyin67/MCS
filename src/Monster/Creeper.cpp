#include "Creeper.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Creeper::Creeper()
{
	auto cfg = loadJsonConfig("assets/config/monster/creeper.json");
	speed_ = cfg.value("speed", 150.0f);
	health_ = cfg.value("health", 30);
	maxHealth_ = cfg.value("maxHealth", 30);
	damage_ = cfg.value("damage", 1);
	expReward_ = cfg.value("expReward", 8);
	std::string texId = cfg.value("textureId", "creeper");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
	flashing_ = false;
	flashTimer_ = 0.0f;
	fuseTime_ = cfg.value("fuseTime", 1.5f);
	triggerDist_ = cfg.value("triggerDistance", 200.0f);
	escapeDist_ = cfg.value("escapeDistance", 300.0f);
	explosionRadius_ = cfg.value("explosionRadius", 150.0f);
	explosionDamage_ = cfg.value("explosionDamage", 3.0f);
}
void Creeper::playHitSound()
{
	SoundManager::getInstance().playSFX("creeper_hurt");
}
void Creeper::playDeathSound()
{
	SoundManager::getInstance().playSFX("creeper_death");
}

void Creeper::update(float deltaTime, const sf::Vector2f &playerPos,
					  const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
	Monster::update(deltaTime, playerPos, allMonsters);

	sf::Vector2f toPlayer = playerPos - position_;
	float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

	if (!flashing_ && dist < triggerDist_)
	{
		flashing_ = true;
		flashTimer_ = 0.0f;
		SoundManager::getInstance().playSFX("creeper_fuse");
	}
	else if (flashing_)
	{
		flashTimer_ += deltaTime;
		if (dist > escapeDist_)
		{
			flashing_ = false;
			flashTimer_ = 0.0f;
			shape_.setFillColor(sf::Color::White);
		}
		else if (flashTimer_ >= fuseTime_)
		{
			SoundManager::getInstance().playSFX("tnt_explode");
			if (Monster::spawnAreaCallback)
				Monster::spawnAreaCallback(position_, explosionRadius_, 0.5f, BuffType::Burn, static_cast<int>(explosionDamage_));
			health_ = 0;
			flashing_ = false;
		}
	}
}

void Creeper::render(sf::RenderWindow &window)
{
	if (flashing_ && flashTimer_ < fuseTime_)
	{
		float cycle = flashTimer_ / fuseTime_;
		int phase = static_cast<int>(cycle * 10) % 2;
		shape_.setFillColor(phase == 0 ? sf::Color::White : sf::Color(230, 230, 230));

		sf::CircleShape warning(explosionRadius_);
		warning.setPosition(position_ - sf::Vector2f(explosionRadius_, explosionRadius_));
		warning.setFillColor(sf::Color::Transparent);
		float alpha = 0.3f + 0.3f * (flashTimer_ / fuseTime_);
		warning.setOutlineColor(sf::Color(255, 60, 60, static_cast<uint8_t>(255 * alpha)));
		warning.setOutlineThickness(2.0f);
		window.draw(warning);
	}
	else if (!flashing_)
	{
		shape_.setFillColor(sf::Color::White);
	}
	Monster::render(window);
}

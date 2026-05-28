#include "Creeper.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Creeper::Creeper()
{
	speed_ = 150.0f;
	health_ = 30;
	maxHealth_ = 30;
	damage_ = 1;
	expReward_ = 8;
	flashing_ = false;
	flashTimer_ = 0.0f;
	fuseTime_ = 1.5f;
	if (ResourceManager::GetInstance().HasTexture("creeper"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("creeper");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});
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

	float triggerDist = 200.0f;
	float escapeDist = 300.0f;

	if (!flashing_ && dist < triggerDist)
	{
		flashing_ = true;
		flashTimer_ = 0.0f;
	}
	else if (flashing_)
	{
		flashTimer_ += deltaTime;
		if (dist > escapeDist)
		{
			flashing_ = false;
			flashTimer_ = 0.0f;
			shape_.setFillColor(sf::Color::White);
		}
		else if (flashTimer_ >= fuseTime_)
		{
			SoundManager::getInstance().playSFX("tnt_explode");
			if (Monster::spawnAreaCallback)
				Monster::spawnAreaCallback(position_, 150.0f, 0.5f, 20, (int)MonsterProjectile::AreaType::Wither);
			health_ = 0;
			flashing_ = false;
		}
	}
}

void Creeper::render(sf::RenderWindow &window)
{
	if (flashing_ && flashTimer_ < fuseTime_)
	{
		// 纯白闪烁
		float cycle = flashTimer_ / fuseTime_;
		int phase = static_cast<int>(cycle * 10) % 2;
		shape_.setFillColor(phase == 0 ? sf::Color::White : sf::Color(230, 230, 230));

		// 绘制爆炸范围预警圈
		sf::CircleShape warning(150.0f);
		warning.setPosition(position_ - sf::Vector2f(150.0f, 150.0f));
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

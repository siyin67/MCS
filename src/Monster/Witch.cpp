#include "Witch.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <cmath>

Witch::Witch()
{
	speed_ = 90.0f;
	health_ = 35;
	maxHealth_ = 35;
	damage_ = 0;
	expReward_ = 8;
	if (ResourceManager::GetInstance().HasTexture("witch"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("witch");
		shape_.setTexture(&tex);
	}
	shape_.setSize({64, 64});

	setSkill([](Monster &self, const sf::Vector2f &playerPos)
	{
		(void)playerPos;
		self.shouldSpawnEffect_ = true;
		self.effectPosition_ = self.getPosition();
		SoundManager::getInstance().playSFX("witch_heal");
	}, 5.0f);
}
void Witch::playHitSound()
{
	SoundManager::getInstance().playSFX("witch_hurt");
}
void Witch::playDeathSound()
{
	SoundManager::getInstance().playSFX("witch_death");
}
void Witch::update(float deltaTime, const sf::Vector2f &playerPos,
					const std::vector<std::unique_ptr<Monster>> &allMonsters)
{
	Monster::update(deltaTime, playerPos, allMonsters);

	if (shouldSpawnEffect_)
	{
		shouldSpawnEffect_ = false;
		float healRange = 250.0f;
		int healAmount = 5;

		for (const auto &m : allMonsters)
		{
			if (!m->isAlive() || m.get() == this)
				continue;
			sf::Vector2f delta = m->getPosition() - position_;
			float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
			if (dist <= healRange)
				m->heal(healAmount);
		}
	}
}
void Witch::render(sf::RenderWindow &window)
{
	Monster::render(window);

	if (skillTimer_ > skillCooldown_ - 1.0f)
	{
		float alpha = std::min(1.0f, (skillCooldown_ - skillTimer_) / 0.5f);
		sf::CircleShape aura(250.0f);
		aura.setPosition(position_ - sf::Vector2f(250.0f, 250.0f));
		aura.setFillColor(sf::Color::Transparent);
		sf::Color green(0, 255, 0, static_cast<uint8_t>(100 * alpha));
		aura.setOutlineColor(green);
		aura.setOutlineThickness(2.0f);
		window.draw(aura);
	}
}

#include "Witch.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>

Witch::Witch()
{
	auto cfg = loadJsonConfig("assets/config/monster/witch.json");
	speed_ = cfg.value("speed", 90.0f);
	health_ = cfg.value("health", 35);
	maxHealth_ = cfg.value("maxHealth", 35);
	damage_ = cfg.value("damage", 0);
	expReward_ = cfg.value("expReward", 8);
	std::string texId = cfg.value("textureId", "witch");
	if (ResourceManager::GetInstance().HasTexture(texId))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
		shape_.setTexture(&tex);
	}
	shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});

	float skillCD = cfg.value("skillCooldown", 5.0f);
	healRange_ = cfg.value("healRange", 250.0f);
	healAmount_ = cfg.value("healAmount", 5);
	setSkill([this](Monster &self, const sf::Vector2f &playerPos)
	{
		(void)playerPos;
		self.shouldSpawnEffect_ = true;
		self.effectPosition_ = self.getPosition();
		SoundManager::getInstance().playSFX("witch_heal");
	}, skillCD);
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

		for (const auto &m : allMonsters)
		{
			if (!m->isAlive() || m.get() == this)
				continue;
			sf::Vector2f delta = m->getPosition() - position_;
			float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
			if (dist <= healRange_)
				m->heal(healAmount_);
		}
	}
}
void Witch::render(sf::RenderWindow &window)
{
	Monster::render(window);

	if (skillTimer_ > skillCooldown_ - 1.0f)
	{
		float alpha = std::min(1.0f, (skillCooldown_ - skillTimer_) / 0.5f);
		sf::CircleShape aura(healRange_);
		aura.setPosition(position_ - sf::Vector2f(healRange_, healRange_));
		aura.setFillColor(sf::Color::Transparent);
		sf::Color green(0, 255, 0, static_cast<uint8_t>(100 * alpha));
		aura.setOutlineColor(green);
		aura.setOutlineThickness(2.0f);
		window.draw(aura);
	}
}

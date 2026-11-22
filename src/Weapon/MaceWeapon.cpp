#include "MaceWeapon.h"
#include "../Monster/Monster.h"
#include "../Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>

MaceWeapon::MaceWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/mace.json");
	name_ = cfg.value("name", "Mace");
	textureId_ = cfg.value("textureId", "mace");
	baseDamage_ = cfg.value("baseDamage", 25);
	damage_ = cfg.value("damage", 25);
	attackCooldown_ = cfg.value("attackCooldown", 2.0f);
	range_ = cfg.value("range", 400.0f);
	level_ = 1;
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});

	attackTimer_ = 0.0f;
	isAttacking_ = false;

	currentFrame_ = 0;
	frameTimer_ = 0.0f;
	isAnimating_ = false;

	for (int i = 0; i <= 11; i++)
	{
		std::string path = "assets/images/effects/mace/gust_" + std::to_string(i) + ".png";
		sf::Texture tex;
		if (tex.loadFromFile(path))
		{
			effectTextures_.push_back(tex);
		}
	}

	if (!effectTextures_.empty())
	{
		effectShape_.setTexture(&effectTextures_[0]);
	}
	effectShape_.setSize({32, 32});
	effectShape_.setOrigin({16, 16});
}

void MaceWeapon::update(float deltaTime)
{
	Weapon::update(deltaTime);

	if (isAttacking_)
	{
		attackTimer_ -= deltaTime;
		if (attackTimer_ <= 0.0f)
			isAttacking_ = false;
	}

	if (isAnimating_)
	{
		frameTimer_ += deltaTime;
		if (frameTimer_ >= 0.05f)
		{
			frameTimer_ = 0.0f;
			currentFrame_++;
			if (currentFrame_ >= (int)effectTextures_.size())
			{
				isAnimating_ = false;
			}
			else
			{
				effectShape_.setTexture(&effectTextures_[currentFrame_]);
			}
		}
	}
}

void MaceWeapon::attack(const sf::Vector2f &playerPos,
						const sf::Vector2f &attackDir,
						std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)attackDir;

	if (!canAttack())
		return;

	isAttacking_ = true;
	attackTimer_ = 0.2f;

	isAnimating_ = true;
	currentFrame_ = 0;
	frameTimer_ = 0.0f;
	if (!effectTextures_.empty())
		effectShape_.setTexture(&effectTextures_[0]);

	SoundManager::getInstance().playSFX("mace_swing");

	bool hitAny = false;
	for (auto &monster : monsters)
	{
		if (!monster->isAlive())
			continue;

		float dx = monster->getPosition().x - playerPos.x;
		float dy = monster->getPosition().y - playerPos.y;
		float distance = std::sqrt(dx * dx + dy * dy);

		if (distance <= range_)
		{
			monster->takeDamage(damage_);
			hitAny = true;
		}
	}

	if (hitAny)
		SoundManager::getInstance().playSFX("mace_hit");

	resetCooldown();
}

void MaceWeapon::render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const
{
	if (!isAnimating_ || effectTextures_.empty())
		return;

	sf::RectangleShape shape = effectShape_;
	shape.setTexture(&effectTextures_[currentFrame_]);

	float diameter = range_ * 1.6f;
	shape.setSize({diameter, diameter});
	shape.setOrigin({diameter / 2.0f, diameter / 2.0f});
	shape.setPosition(playerPos);

	window.draw(shape);
}

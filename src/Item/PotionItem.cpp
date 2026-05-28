#include "PotionItem.h"
#include "../Resourcemanager/ResourceManager.h"
#include "../Player/player.h"
#include <cmath>

PotionItem::PotionItem(sf::Vector2f pos, BuffType type, float value, float duration)
	: DropItem(pos, 0, 60.0f), buffType_(type), buffValue_(value), buffDuration_(duration),
	  attractTimer_(0.0f)
{
	shape_.setSize({24, 24});
	shape_.setOrigin({12, 12});
}

void PotionItem::update(float deltaTime, const sf::Vector2f &playerPos)
{
	elapsed_ += deltaTime;
	if (elapsed_ >= lifetime_)
	{
		alive_ = false;
		return;
	}

	// 渐隐
	if (elapsed_ > lifetime_ - 5.0f)
	{
		float alpha = 1.0f - (elapsed_ - (lifetime_ - 5.0f)) / 5.0f;
		shape_.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255 * alpha)));
	}

	// 吸引效果
	attractTimer_ += deltaTime;
	if (attractTimer_ > 1.0f)
	{
		sf::Vector2f toPlayer = playerPos - position_;
		float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
		if (dist < 350.0f && dist > 1.0f)
		{
			float speed = 500.0f * (1.0f - dist / 350.0f) + 100.0f;
			position_ += toPlayer / dist * speed * deltaTime;
		}
	}

	shape_.setPosition(position_);
}

void PotionItem::render(sf::RenderWindow &window)
{
	if (!alive_)
		return;

	float size = 28.0f;

	// 基础药水瓶
	if (ResourceManager::GetInstance().HasTexture("potion"))
	{
		sf::Sprite bottle(ResourceManager::GetInstance().GetTexture("potion"));
		bottle.setScale({size / bottle.getTexture().getSize().x,
						 size / bottle.getTexture().getSize().y});
		bottle.setPosition(position_ - sf::Vector2f(size / 2.0f, size / 2.0f));
		window.draw(bottle);
	}

	// 彩色 overlay
	if (ResourceManager::GetInstance().HasTexture("potion_overlay"))
	{
		sf::Sprite overlay(ResourceManager::GetInstance().GetTexture("potion_overlay"));
		overlay.setScale({size / overlay.getTexture().getSize().x,
						  size / overlay.getTexture().getSize().y});
		overlay.setPosition(position_ - sf::Vector2f(size / 2.0f, size / 2.0f));

		switch (buffType_)
		{
		case BuffType::Heal:       overlay.setColor(sf::Color(253, 75, 107)); break;   // 红粉 - 瞬间治疗
		case BuffType::SpeedUp:    overlay.setColor(sf::Color(124, 175, 255)); break;  // 浅蓝 - 速度
		case BuffType::AttackUp:   overlay.setColor(sf::Color(150, 40, 40)); break;    // 深红 - 力量
		case BuffType::Invincible: overlay.setColor(sf::Color(100, 110, 140)); break;  // 灰蓝 - 抗性
		default:                   overlay.setColor(sf::Color(255, 100, 150)); break;  // 默认粉
		}
		window.draw(overlay);
	}

	// 小图标
	const char *iconId = nullptr;
	switch (buffType_)
	{
	case BuffType::Heal:       iconId = "buff_instant_health"; break;
	case BuffType::SpeedUp:    iconId = "buff_speed_up"; break;
	case BuffType::AttackUp:   iconId = "buff_attack_up"; break;
	case BuffType::Invincible: iconId = "buff_invincible"; break;
	default: break;
	}
	if (iconId && ResourceManager::GetInstance().HasTexture(iconId))
	{
		float iconSize = size * 0.55f;
		sf::Sprite icon(ResourceManager::GetInstance().GetTexture(iconId));
		icon.setScale({iconSize / icon.getTexture().getSize().x,
					   iconSize / icon.getTexture().getSize().y});
		icon.setPosition(position_ - sf::Vector2f(iconSize / 2.0f, iconSize / 2.0f));
		window.draw(icon);
	}
}

void PotionItem::onCollect(Player &player)
{
	if (buffType_ == BuffType::Heal && buffDuration_ <= 0.1f)
	{
		// 瞬间治疗：直接回血
		player.heal(static_cast<int>(buffValue_));
	}
	else
	{
		player.getBuffManager().add(buffType_, buffValue_, buffDuration_);
	}
}

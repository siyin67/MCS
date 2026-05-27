#include "DragonBreathArea.h"
#include "Monster/Monster.h"
#include <cmath>
#include <algorithm>

DragonBreathArea::DragonBreathArea(sf::Vector2f pos, int damage, float radius, float duration)
	: radius_(radius), duration_(duration),
	  elapsed_(0.0f), damageTickTimer_(0.0f), shouldDamageTick_(false)
{
	damage_ = damage;
	isActive_ = true;

	position_ = pos;
	// shape_ 设为0大小避免怪物碰撞误触发 setActive(false)
	shape_.setSize({0, 0});
	shape_.setPosition({-1000, -1000});

	circle_.setRadius(radius_);
	circle_.setOrigin({radius_, radius_});
	circle_.setPosition(position_);
	circle_.setFillColor(sf::Color(160, 120, 255, 60));
	circle_.setOutlineColor(sf::Color(180, 140, 255, 100));
	circle_.setOutlineThickness(3.0f);
}

void DragonBreathArea::update(float deltaTime)
{
	if (!isActive_)
		return;

	elapsed_ += deltaTime;
	damageTickTimer_ += deltaTime;

	if (damageTickTimer_ >= DAMAGE_INTERVAL)
	{
		damageTickTimer_ -= DAMAGE_INTERVAL;
		shouldDamageTick_ = true;
		hitSet_.clear();
	}

	if (elapsed_ >= duration_)
	{
		isActive_ = false;
		if (onDestroy_)
			onDestroy_();
	}
}

void DragonBreathArea::render(sf::RenderWindow &window)
{
	if (!isActive_)
		return;
	window.draw(circle_);
}

void DragonBreathArea::onHit(Monster &monster)
{
	(void)monster;
}

void DragonBreathArea::checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (!isActive_ || !shouldDamageTick_)
		return;
	shouldDamageTick_ = false;

	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;

		sf::Vector2f delta = m->getPosition() - position_;
		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (dist <= radius_)
		{
			if (std::find(hitSet_.begin(), hitSet_.end(), m.get()) != hitSet_.end())
				continue;
			hitSet_.push_back(m.get());
			m->takeDamage(damage_);
		}
	}
}

sf::FloatRect DragonBreathArea::getGlobalBounds() const
{
	return sf::FloatRect({0, 0}, {0, 0});
}

void DragonBreathArea::setTrackingTarget(const sf::Vector2f &target)
{
	position_ = target;
	circle_.setPosition(position_);
}

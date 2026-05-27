#include "AxeProjectile.h"
#include "Monster/Monster.h"
#include "Resourcemanager/ResourceManager.h"
#include <cmath>
#include <algorithm>

AxeProjectile::AxeProjectile(sf::Vector2f startPos, sf::Vector2f dir,
							   int damage, float maxDist, float speed, float radius)
	: state_(AxeState::FLYING),
	  flyDir_(dir), maxDistance_(maxDist),
	  traveledDist_(0.0f), speed_(speed),
	  spinRadius_(radius),
	  spinTimer_(SPIN_DURATION),
	  damageTickTimer_(0.0f),
	  shouldDamageTick_(false)
{
	damage_ = damage;
	isActive_ = true;

	setTextureId("axe");
	setSize({64, 64});
	shape_.setFillColor(sf::Color::White);

	position_ = startPos;
	shape_.setPosition(position_);

	float angleDeg = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;
	shape_.setRotation(sf::degrees(angleDeg + 45.0f));
}

void AxeProjectile::update(float deltaTime)
{
	if (!isActive_)
		return;

	switch (state_)
	{
	case AxeState::FLYING:
	{
		position_ += flyDir_ * speed_ * deltaTime;
		traveledDist_ += speed_ * deltaTime;
		if (traveledDist_ >= maxDistance_)
		{
			state_ = AxeState::SPINNING;
			hitSet_.clear();
			spinTimer_ = SPIN_DURATION;
			damageTickTimer_ = 0.0f;
		}
		break;
	}
	case AxeState::SPINNING:
	{
		spinTimer_ -= deltaTime;
		damageTickTimer_ += deltaTime;

		if (damageTickTimer_ >= DAMAGE_INTERVAL)
		{
			damageTickTimer_ -= DAMAGE_INTERVAL;
			shouldDamageTick_ = true;
			hitSet_.clear();
		}

		// 旋转动画
		shape_.rotate(sf::degrees(720.0f * deltaTime));

		if (spinTimer_ <= 0.0f)
		{
			state_ = AxeState::DEAD;
			isActive_ = false;
		}
		break;
	}
	case AxeState::DEAD:
		isActive_ = false;
		break;
	}

	shape_.setPosition(position_);
}

void AxeProjectile::render(sf::RenderWindow &window)
{
	if (!isActive_)
		return;
	window.draw(shape_);
}

void AxeProjectile::onHit(Monster &monster)
{
	if (state_ != AxeState::FLYING)
		return;

	if (std::find(hitSet_.begin(), hitSet_.end(), &monster) != hitSet_.end())
		return;

	hitSet_.push_back(&monster);
	monster.takeDamage(damage_);

	// 命中后开始旋转
	state_ = AxeState::SPINNING;
	hitSet_.clear();
	spinTimer_ = SPIN_DURATION;
	damageTickTimer_ = 0.0f;
}

void AxeProjectile::checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (state_ != AxeState::SPINNING)
		return;
	if (!shouldDamageTick_)
		return;
	shouldDamageTick_ = false;

	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;

		sf::Vector2f delta = m->getPosition() - position_;
		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (dist <= spinRadius_)
		{
			if (std::find(hitSet_.begin(), hitSet_.end(), m.get()) != hitSet_.end())
				continue;
			hitSet_.push_back(m.get());
			m->takeDamage(damage_ / 2); // 旋转伤害为直接命中的一半
		}
	}
}

void AxeProjectile::setActive(bool active)
{
	if (!active && state_ != AxeState::DEAD)
		return;
	Projectile::setActive(active);
}

sf::FloatRect AxeProjectile::getGlobalBounds() const
{
	if (state_ == AxeState::FLYING)
		return shape_.getGlobalBounds();
	return sf::FloatRect({0, 0}, {0, 0});
}

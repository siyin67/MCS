#include "BubbleProjectile.h"
#include "../Monster/Monster.h"
#include <cmath>

BubbleProjectile::BubbleProjectile()
	: speed_(600.0f), maxBounces_(3), bounceCount_(0), hasTarget_(false)
{
	setSize({24, 24});
	setTextureId("bubble_button");
	damage_ = 3;
	lifetime_ = 10.0f;
}

void BubbleProjectile::findNextTarget(const std::vector<std::unique_ptr<Monster>> &monsters)
{
	float closestDist = 99999.0f;
	Monster *closest = nullptr;

	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;
		bool alreadyHit = false;
		for (auto *h : hitList_)
		{
			if (h == m.get())
			{
				alreadyHit = true;
				break;
			}
		}
		if (alreadyHit)
			continue;

		sf::Vector2f d = m->getPosition() - position_;
		float dist = d.x * d.x + d.y * d.y;
		if (dist < closestDist)
		{
			closestDist = dist;
			closest = m.get();
		}
	}

	if (closest)
	{
		targetPos_ = closest->getPosition();
		hasTarget_ = true;
	}
	else
	{
		hasTarget_ = false;
	}
}

void BubbleProjectile::update(float deltaTime)
{
	if (!isActive_)
		return;

	if (hasTarget_)
	{
		sf::Vector2f dir = targetPos_ - position_;
		float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (dist > 1.0f)
		{
			dir /= dist;
			position_ += dir * speed_ * deltaTime;
		}
	}
	shape_.setPosition(position_);

	lifetime_ -= deltaTime;
	if (lifetime_ <= 0.0f)
		isActive_ = false;
}

void BubbleProjectile::updateTargeting(const std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (!isActive_)
		return;

	// 找到最近的未命中怪物，更新目标位置
	Monster *closest = nullptr;
	float closestDist = 30.0f; // 命中判定距离

	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;
		bool alreadyHit = false;
		for (auto *h : hitList_)
		{
			if (h == m.get()) { alreadyHit = true; break; }
		}
		if (alreadyHit)
			continue;

		sf::Vector2f d = m->getPosition() - position_;
		float dist = std::sqrt(d.x * d.x + d.y * d.y);
		if (dist < closestDist)
		{
			closestDist = dist;
			closest = m.get();
		}
	}

	if (closest)
	{
		// 距离足够近 → 命中
		if (closestDist < 30.0f)
		{
			onHit(*closest);
			if (!isActive_)
				return;
		}
		targetPos_ = closest->getPosition();
		hasTarget_ = true;
	}
	else
	{
		findNextTarget(monsters);
		if (!hasTarget_)
			isActive_ = false;
	}
}

void BubbleProjectile::onHit(Monster &monster)
{
	monster.takeDamage(damage_);
	hitList_.push_back(&monster);
	bounceCount_++;
	if (bounceCount_ >= maxBounces_)
		isActive_ = false;
}

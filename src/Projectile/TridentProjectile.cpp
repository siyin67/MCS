#include "TridentProjectile.h"
#include "Monster/Monster.h"
#include "Resourcemanager/ResourceManager.h"
#include <cmath>
#include <algorithm>

TridentProjectile::TridentProjectile(sf::Vector2f startPos, sf::Vector2f dir,
									 int damage, float maxDist, float speed)
	: state_(TridentState::FLYING_OUT),
	  flyDir_(dir), maxDistance_(maxDist),
	  traveledDist_(0.0f), speed_(speed),
	  playerPos_(startPos) // 初始化为发射位置
{
	damage_ = damage;
	isActive_ = true;

	setTextureId("trident");
	setSize({48, 48});
	shape_.setFillColor(sf::Color(150, 200, 255));

	position_ = startPos;
	shape_.setPosition(position_);

	float angleDeg = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;
	shape_.setRotation(sf::degrees(angleDeg + 45.0f));
}

void TridentProjectile::update(float deltaTime)
{
	if (!isActive_)
		return;

	switch (state_)
	{
	case TridentState::FLYING_OUT:
	{
		position_ += flyDir_ * speed_ * deltaTime;
		traveledDist_ += speed_ * deltaTime;
		if (traveledDist_ >= maxDistance_)
		{
			state_ = TridentState::RETURNING;
			hitSet_.clear();
		}
		break;
	}
	case TridentState::RETURNING:
	{
		// 朝玩家当前位置飞行
		sf::Vector2f toPlayer = playerPos_ - position_;
		float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
		if (dist < 40.0f)
		{
			state_ = TridentState::DEAD;
			isActive_ = false;
			if (onDestroy_)
				onDestroy_();
			break;
		}
		toPlayer.x /= dist;
		toPlayer.y /= dist;
		flyDir_ = toPlayer;
		position_ += flyDir_ * speed_ * deltaTime;
		break;
	}
	case TridentState::DEAD:
		isActive_ = false;
		break;
	}

	shape_.setPosition(position_);

	// 旋转跟随飞行方向
	float angleDeg = std::atan2(flyDir_.y, flyDir_.x) * 180.0f / 3.14159f;
	shape_.setRotation(sf::degrees(angleDeg + 45.0f));
}

void TridentProjectile::render(sf::RenderWindow &window)
{
	if (!isActive_)
		return;
	window.draw(shape_);
}

void TridentProjectile::onHit(Monster &monster)
{
	if (std::find(hitSet_.begin(), hitSet_.end(), &monster) != hitSet_.end())
		return;

	hitSet_.push_back(&monster);
	monster.takeDamage(damage_);
}

void TridentProjectile::setActive(bool active)
{
	if (!active && state_ != TridentState::DEAD)
		return;
	Projectile::setActive(active);
}

void TridentProjectile::setTrackingTarget(const sf::Vector2f &target)
{
	playerPos_ = target;
}
#pragma once
#include "Projectile.h"
#include <vector>
#include <functional>

class Monster;

enum class TridentState
{
	FLYING_OUT,
	RETURNING,
	DEAD
};

class TridentProjectile : public Projectile
{
private:
	TridentState state_;
	sf::Vector2f flyDir_;
	float maxDistance_;
	float traveledDist_;
	float speed_;
	sf::Vector2f playerPos_; // 每帧更新的玩家位置
	std::vector<Monster *> hitSet_;
	std::function<void()> onDestroy_;

public:
	TridentProjectile(sf::Vector2f startPos, sf::Vector2f dir,
					  int damage, float maxDist, float speed);
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window) override;
	void onHit(Monster &monster) override;
	void setActive(bool active) override;
	void setTrackingTarget(const sf::Vector2f &target) override;
	void setOnDestroy(std::function<void()> callback) { onDestroy_ = std::move(callback); }
};
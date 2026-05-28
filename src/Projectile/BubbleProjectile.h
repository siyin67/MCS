#pragma once
#include "Projectile.h"
#include <vector>
#include <memory>

class BubbleProjectile : public Projectile
{
private:
	sf::Vector2f targetPos_;
	float speed_;
	int maxBounces_;
	int bounceCount_;
	std::vector<class Monster *> hitList_;
	bool hasTarget_;

	void findNextTarget(const std::vector<std::unique_ptr<Monster>> &monsters);

public:
	BubbleProjectile();

	void setTargetPos(const sf::Vector2f &pos) { targetPos_ = pos; hasTarget_ = true; }
	void setSpeed(float s) { speed_ = s; }
	void setMaxBounces(int n) { maxBounces_ = n; }

	void update(float deltaTime) override;
	void updateTargeting(const std::vector<std::unique_ptr<Monster>> &monsters);
	void onHit(Monster &monster) override;
	bool usesCustomTargeting() const override { return true; }
};

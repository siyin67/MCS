#pragma once
#include "Projectile.h"
#include <vector>

struct LavaFloor
{
	sf::Vector2f position;
	float timer;
	int dps;
	float damageAccum;
};

class LavaProjectile : public Projectile
{
public:
	static inline std::vector<LavaFloor> pendingFloors;

private:
	sf::Vector2f targetPos_;
	float speed_;
	bool landed_;
	int floorDamage_;

public:
	LavaProjectile(sf::Vector2f startPos, sf::Vector2f targetPos, int damage, float speed);

	void update(float deltaTime) override;
	void onHit(Monster &monster) override;
	bool hasLanded() const { return landed_; }
	bool usesCustomTargeting() const override { return true; }
};

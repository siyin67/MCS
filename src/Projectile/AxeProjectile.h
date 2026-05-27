#pragma once
#include "Projectile.h"
#include <vector>

class Monster;

enum class AxeState
{
	FLYING,
	SPINNING,
	DEAD
};

class AxeProjectile : public Projectile
{
private:
	AxeState state_;
	sf::Vector2f flyDir_;
	float maxDistance_;
	float traveledDist_;
	float speed_;
	float spinRadius_;
	float spinTimer_;
	float damageTickTimer_;
	bool shouldDamageTick_;
	std::vector<Monster *> hitSet_;

	static constexpr float SPIN_DURATION = 3.0f;
	static constexpr float DAMAGE_INTERVAL = 1.0f / 3.0f;

public:
	AxeProjectile(sf::Vector2f startPos, sf::Vector2f dir,
				  int damage, float maxDist, float speed, float radius);
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window) override;
	void onHit(Monster &monster) override;
	void checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters) override;
	void setActive(bool active) override;
	sf::FloatRect getGlobalBounds() const override;
};

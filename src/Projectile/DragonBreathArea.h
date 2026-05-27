#pragma once
#include "Projectile.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

class Monster;

class DragonBreathArea : public Projectile
{
private:
	float radius_;
	float duration_;
	float elapsed_;
	float damageTickTimer_;
	bool shouldDamageTick_;
	std::vector<Monster *> hitSet_;
	sf::CircleShape circle_;
	std::function<void()> onDestroy_;

	static constexpr float DAMAGE_INTERVAL = 0.5f;

public:
	DragonBreathArea(sf::Vector2f pos, int damage, float radius, float duration);
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window) override;
	void onHit(Monster &monster) override;
	void checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters) override;
	sf::FloatRect getGlobalBounds() const override;
	void setTrackingTarget(const sf::Vector2f &target) override;
	void setOnDestroy(std::function<void()> callback) { onDestroy_ = std::move(callback); }
};

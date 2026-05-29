#pragma once
#include "Weapon.h"
#include <SFML/Graphics.hpp>
#include <vector>

class MaceWeapon : public Weapon
{
private:
	float attackTimer_;
	bool isAttacking_;

	std::vector<sf::Texture> effectTextures_;
	int currentFrame_;
	float frameTimer_;
	bool isAnimating_;
	sf::RectangleShape effectShape_;

public:
	MaceWeapon();
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const;
};

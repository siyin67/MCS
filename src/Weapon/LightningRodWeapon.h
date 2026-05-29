#pragma once
#include "Weapon.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class LightningRodWeapon : public Weapon
{
private:
	struct LightningBolt
	{
		sf::Vector2f from;
		sf::Vector2f to;
		std::vector<sf::Vector2f> points;
	};
	std::vector<LightningBolt> currentBolts_;
	float displayTimer_;

	int chainCount_;
	float chainRadius_;
	float chainDamagePercent_;

	std::mt19937 rng_;

	std::vector<sf::Vector2f> generatePoints(sf::Vector2f from, sf::Vector2f to);

public:
	LightningRodWeapon();
	void attack(const sf::Vector2f &playerPos,
				const sf::Vector2f &attackDir,
				std::vector<std::unique_ptr<Monster>> &monsters) override;
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const;
};

#pragma once
#include "Projectile.h"
#include <SFML/Graphics.hpp>
#include <array>

enum class TNTState
{
	FLYING,    // 飞行中
	LANDED,    // 落地闪烁
	EXPLODING, // 爆炸动画
	DEAD       // 结束
};

class TNTProjectile : public Projectile
{
private:
	TNTState state_;
	sf::Vector2f targetPos_;
	float speed_;
	float explosionRadius_;
	float stateTimer_;

	// 闪烁
	bool flashOn_;
	float flashTimer_;

	// 爆炸动画（16帧）
	std::array<sf::Texture, 16> explosionTex_;
	int explosionFrame_;
	float explosionFrameTimer_;
	bool damageDealt_;
	static constexpr int EXPLOSION_FRAMES = 16;
	static constexpr float LANDED_DURATION = 0.5f;
	static constexpr float EXPLOSION_DURATION = 0.5f;

public:
	TNTProjectile(sf::Vector2f startPos, sf::Vector2f targetPos,
				  int damage, float radius);
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window) override;
	void onHit(Monster &monster) override;
	void checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters) override;
	sf::FloatRect getGlobalBounds() const override;
	sf::FloatRect getBounds() const override;
};

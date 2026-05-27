#include "TNTWeapon.h"
#include "../Projectile/TNTProjectile.h"
#include <cmath>
#include <random>

TNTWeapon::TNTWeapon()
{
	name_ = "TNT";
	textureId_ = "tnt";
	baseDamage_ = 30;
	damage_ = 30;
	attackCooldown_ = 2.0f;
	range_ = 350.0f;
	level_ = 1;

	explosionRadius_ = 120.0f;
	projectileSpeed_ = 500.0f;
	throwCount_ = 1;
}

void TNTWeapon::attack(const sf::Vector2f &playerPos,
					   const sf::Vector2f &attackDir,
					   std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)monsters;
	(void)attackDir;
	if (!canAttack())
		return;

	static std::random_device rd;
	static std::mt19937 rng(rd());

	const float minDist = 80.0f; // 不会生成在玩家附近
	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
	std::uniform_real_distribution<float> distDist(minDist, range_);

	for (int i = 0; i < throwCount_; i++)
	{
		float angle = angleDist(rng);
		float dist = distDist(rng);
		sf::Vector2f target = playerPos + sf::Vector2f(std::cos(angle), std::sin(angle)) * dist;

		auto tnt = std::make_unique<TNTProjectile>(
			playerPos, target, damage_, explosionRadius_);
		addProjectile(std::move(tnt));
	}

	resetCooldown();
}

void TNTWeapon::levelUp()
{
	Weapon::levelUp();

	damage_ += 10;
	explosionRadius_ += 15.0f;
	attackCooldown_ = std::max(0.8f, attackCooldown_ - 0.1f);

	// 每2级增加投掷数量
	if (level_ % 2 == 0 && throwCount_ < 6)
		throwCount_++;
}
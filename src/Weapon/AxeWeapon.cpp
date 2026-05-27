#include "AxeWeapon.h"
#include "../Projectile/AxeProjectile.h"
#include "../Monster/Monster.h"
#include <cmath>

AxeWeapon::AxeWeapon()
{
	name_ = "Axe";
	textureId_ = "axe";
	baseDamage_ = 35;
	damage_ = 35;
	attackCooldown_ = 2.0f;
	range_ = 350.0f;
	level_ = 1;
	projectileSpeed_ = 500.0f;
	spinRadius_ = 80.0f;
}

static float distSq(const sf::Vector2f &a, const sf::Vector2f &b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx * dx + dy * dy;
}

void AxeWeapon::attack(const sf::Vector2f &playerPos,
						const sf::Vector2f &attackDir,
						std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (!canAttack())
		return;

	Monster *closest = nullptr;
	float closestDist = 0.0f;
	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;
		float d = distSq(playerPos, m->getPosition());
		if (!closest || d < closestDist)
		{
			closest = m.get();
			closestDist = d;
		}
	}

	sf::Vector2f dir;
	if (closest)
	{
		dir = closest->getPosition() - playerPos;
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len > 0.01f)
		{
			dir.x /= len;
			dir.y /= len;
		}
	}
	else
	{
		dir = attackDir;
	}

	addProjectile(std::make_unique<AxeProjectile>(
		playerPos, dir, damage_, range_, projectileSpeed_, spinRadius_));
	resetCooldown();
}

void AxeWeapon::levelUp()
{
	Weapon::levelUp();
	damage_ += 12;
	projectileSpeed_ += 40.0f;
	spinRadius_ += 10.0f;
	attackCooldown_ = std::max(0.5f, attackCooldown_ - 0.1f);
}

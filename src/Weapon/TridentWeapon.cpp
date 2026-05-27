#include "TridentWeapon.h"
#include "../Projectile/TridentProjectile.h"
#include "../Monster/Monster.h"
#include "../Sound/SoundManager.h"
#include <cmath>
#include <algorithm>

TridentWeapon::TridentWeapon()
{
	name_ = "Trident";
	textureId_ = "trident";
	baseDamage_ = 20;
	damage_ = 20;
	attackCooldown_ = 1.5f;
	range_ = 400.0f;
	level_ = 1;

	projectileSpeed_ = 600.0f;
	maxOnField_ = 1;
	activeCount_ = 0;
}

static float distSq(const sf::Vector2f &a, const sf::Vector2f &b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx * dx + dy * dy;
}

void TridentWeapon::attack(const sf::Vector2f &playerPos,
						   const sf::Vector2f &attackDir,
						   std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (!canAttack())
		return;
	if (activeCount_ >= maxOnField_)
		return;

	SoundManager::getInstance().playSFX("trident_throw");

	// 找到最近的活着的怪物
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

	auto trident = std::make_unique<TridentProjectile>(
		playerPos, dir, damage_, range_, projectileSpeed_);
	trident->setOnDestroy([this]() { onTridentDestroyed(); });
	activeCount_++;

	addProjectile(std::move(trident));
	resetCooldown();
}

void TridentWeapon::levelUp()
{
	Weapon::levelUp();

	damage_ += 8;
	projectileSpeed_ += 50.0f;
	range_ += 30.0f;

	// 每3级增加场上一把三叉戟
	if (level_ % 3 == 0 && maxOnField_ < 5)
		maxOnField_++;
}
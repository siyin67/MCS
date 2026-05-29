#include "AxeWeapon.h"
#include "../Projectile/AxeProjectile.h"
#include "../Monster/Monster.h"
#include "../Config/ConfigLoader.h"
#include <cmath>

AxeWeapon::AxeWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/axe.json");
	name_ = cfg.value("name", "Axe");
	textureId_ = cfg.value("textureId", "axe");
	baseDamage_ = cfg.value("baseDamage", 35);
	damage_ = cfg.value("damage", 35);
	attackCooldown_ = cfg.value("attackCooldown", 2.0f);
	range_ = cfg.value("range", 350.0f);
	level_ = 1;
	projectileSpeed_ = cfg.value("projectileSpeed", 500.0f);
	spinRadius_ = cfg.value("spinRadius", 80.0f);
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
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

	auto axe = std::make_unique<AxeProjectile>(
		playerPos, dir, damage_, range_, projectileSpeed_, spinRadius_);
	axe->setFriendly(true);
	addProjectile(std::move(axe));
	resetCooldown();
}

void AxeWeapon::levelUp()
{
	Weapon::levelUp();
	projectileSpeed_ += levelUpCfg_.value("projectileSpeed", 40.0f);
	spinRadius_ += levelUpCfg_.value("spinRadius", 10.0f);
}

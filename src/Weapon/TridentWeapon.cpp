#include "TridentWeapon.h"
#include "../Projectile/TridentProjectile.h"
#include "../Monster/Monster.h"
#include "../Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>
#include <algorithm>

TridentWeapon::TridentWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/trident.json");
	name_ = cfg.value("name", "Trident");
	textureId_ = cfg.value("textureId", "trident");
	baseDamage_ = cfg.value("baseDamage", 20);
	damage_ = cfg.value("damage", 20);
	attackCooldown_ = cfg.value("attackCooldown", 1.5f);
	range_ = cfg.value("range", 400.0f);
	level_ = 1;

	projectileSpeed_ = cfg.value("projectileSpeed", 600.0f);
	maxOnField_ = cfg.value("maxOnField", 1);
	activeCount_ = 0;
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
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
	trident->setFriendly(true);
	trident->setOnDestroy([this]() { onTridentDestroyed(); });
	activeCount_++;

	addProjectile(std::move(trident));
	resetCooldown();
}

void TridentWeapon::levelUp()
{
	Weapon::levelUp();

	projectileSpeed_ += levelUpCfg_.value("projectileSpeed", 50.0f);

	int everyN = levelUpCfg_.value("maxOnFieldEveryN", 3);
	int maxCap = levelUpCfg_.value("maxOnFieldMax", 5);
	if (level_ % everyN == 0 && maxOnField_ < maxCap)
		maxOnField_++;
}

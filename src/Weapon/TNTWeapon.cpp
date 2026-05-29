#include "TNTWeapon.h"
#include "../Projectile/TNTProjectile.h"
#include "../Config/ConfigLoader.h"
#include <cmath>
#include <random>

TNTWeapon::TNTWeapon()
{
    auto cfg = loadJsonConfig("assets/config/weapon/tnt.json");
    name_ = cfg.value("name", "TNT");
    textureId_ = cfg.value("textureId", "tnt");
    baseDamage_ = cfg.value("baseDamage", 30);
    damage_ = cfg.value("damage", 30);
    attackCooldown_ = cfg.value("attackCooldown", 2.0f);
    range_ = cfg.value("range", 350.0f);
    level_ = 1;

    explosionRadius_ = cfg.value("explosionRadius", 120.0f);
    projectileSpeed_ = cfg.value("projectileSpeed", 500.0f);
    throwCount_ = cfg.value("throwCount", 1);
    levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
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
		tnt->setFriendly(true);
		addProjectile(std::move(tnt));
	}

	resetCooldown();
}

void TNTWeapon::levelUp()
{
	Weapon::levelUp();

	explosionRadius_ += levelUpCfg_.value("explosionRadius", 15.0f);

	if (level_ % 2 == 0 && throwCount_ < 6)
		throwCount_++;
}
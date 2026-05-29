#include "LavaWeapon.h"
#include "../Projectile/LavaProjectile.h"
#include "../Config/ConfigLoader.h"

LavaWeapon::LavaWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/lava.json");
	name_ = cfg.value("name", "Lava Bucket");
	textureId_ = cfg.value("textureId", "lava_bucket");
	baseDamage_ = cfg.value("baseDamage", 5);
	damage_ = cfg.value("damage", 5);
	range_ = cfg.value("range", 350.0f);
	attackCooldown_ = cfg.value("attackCooldown", 4.0f);
	currentCooldown_ = 0.0f;
	projectileSpeed_ = cfg.value("projectileSpeed", 500.0f);
	floorDamage_ = cfg.value("floorDamage", 8);
	floorDuration_ = cfg.value("floorDuration", 5.0f);
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
}

void LavaWeapon::update(float deltaTime)
{
	if (currentCooldown_ > 0.0f)
		currentCooldown_ -= deltaTime;
}

void LavaWeapon::attack(const sf::Vector2f &playerPos,
						 const sf::Vector2f &attackDir,
						 std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)monsters;
	if (!canAttack())
		return;

	sf::Vector2f target = playerPos + attackDir * 200.0f;
	auto proj = std::make_unique<LavaProjectile>(playerPos, target, floorDamage_, projectileSpeed_);
	proj->setFriendly(true);
	addProjectile(std::move(proj));
	resetCooldown();
}

void LavaWeapon::levelUp()
{
	Weapon::levelUp();
	floorDamage_ += levelUpCfg_.value("floorDamage", 4);
	floorDuration_ += levelUpCfg_.value("floorDuration", 1.0f);
}

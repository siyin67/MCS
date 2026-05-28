#include "LavaWeapon.h"
#include "../Projectile/LavaProjectile.h"

LavaWeapon::LavaWeapon()
{
	name_ = "Lava Bucket";
	textureId_ = "lava_bucket";
	baseDamage_ = 5;
	damage_ = 5;
	range_ = 350.0f;
	attackCooldown_ = 4.0f;
	currentCooldown_ = 0.0f;
	projectileSpeed_ = 500.0f;
	floorDamage_ = 8;
	floorDuration_ = 5.0f;
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
	damage_ += 3;
	floorDamage_ += 4;
	floorDuration_ += 1.0f;
	attackCooldown_ = std::max(2.0f, attackCooldown_ - 0.3f);
}

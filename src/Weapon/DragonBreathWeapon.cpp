#include "DragonBreathWeapon.h"
#include "../Projectile/DragonBreathArea.h"
#include "../Monster/Monster.h"

DragonBreathWeapon::DragonBreathWeapon()
{
	name_ = "Dragon Breath";
	textureId_ = "dragon";
	baseDamage_ = 8;
	damage_ = 8;
	attackCooldown_ = 4.0f;
	range_ = 0.0f;
	level_ = 1;
	radius_ = 80.0f;
	areaDuration_ = 2.0f;
	maxLayers_ = 1;
	activeCount_ = 0;
}

void DragonBreathWeapon::attack(const sf::Vector2f &playerPos,
								 const sf::Vector2f &attackDir,
								 std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)attackDir;
	(void)monsters;

	if (!canAttack())
		return;
	if (activeCount_ >= maxLayers_)
		return;

	auto area = std::make_unique<DragonBreathArea>(
		playerPos, damage_, radius_, areaDuration_);
	area->setFriendly(true);
	area->setOnDestroy([this]() { onAreaDestroyed(); });
	activeCount_++;

	addProjectile(std::move(area));
	resetCooldown();
}

void DragonBreathWeapon::levelUp()
{
	++level_;

	switch (level_)
	{
	case 2: damage_ = 10; radius_ = 90;  areaDuration_ = 2.2f; attackCooldown_ = 3.8f; maxLayers_ = 1; break;
	case 3: damage_ = 12; radius_ = 100; areaDuration_ = 2.4f; attackCooldown_ = 3.6f; maxLayers_ = 1; break;
	case 4: damage_ = 14; radius_ = 110; areaDuration_ = 2.6f; attackCooldown_ = 3.4f; maxLayers_ = 2; break;
	case 5: damage_ = 16; radius_ = 120; areaDuration_ = 2.8f; attackCooldown_ = 3.2f; maxLayers_ = 2; break;
	case 6: damage_ = 20; radius_ = 130; areaDuration_ = 3.0f; attackCooldown_ = 3.0f; maxLayers_ = 3; break;
	default:
		damage_ += 4;
		radius_ += 10.0f;
		areaDuration_ += 0.2f;
		attackCooldown_ = std::max(1.0f, attackCooldown_ - 0.2f);
		if (level_ % 2 == 0 && maxLayers_ < 6)
			maxLayers_++;
		break;
	}

	baseDamage_ = damage_;
}

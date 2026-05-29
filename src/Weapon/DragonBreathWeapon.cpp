#include "DragonBreathWeapon.h"
#include "../Projectile/DragonBreathArea.h"
#include "../Monster/Monster.h"
#include "../Config/ConfigLoader.h"

DragonBreathWeapon::DragonBreathWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/dragon.json");
	name_ = cfg.value("name", "Dragon Breath");
	textureId_ = cfg.value("textureId", "dragon");
	baseDamage_ = cfg.value("baseDamage", 8);
	damage_ = cfg.value("damage", 8);
	attackCooldown_ = cfg.value("attackCooldown", 4.0f);
	range_ = cfg.value("range", 0.0f);
	level_ = 1;
	radius_ = cfg.value("radius", 80.0f);
	areaDuration_ = cfg.value("areaDuration", 2.0f);
	maxLayers_ = cfg.value("maxLayers", 1);
	activeCount_ = 0;
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
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

	if (!levelUpCfg_.empty() && levelUpCfg_.contains("levels"))
	{
		std::string lvKey = std::to_string(level_);
		if (levelUpCfg_["levels"].contains(lvKey))
		{
			auto &lv = levelUpCfg_["levels"][lvKey];
			damage_ = lv.value("damage", damage_);
			radius_ = lv.value("radius", radius_);
			areaDuration_ = lv.value("areaDuration", areaDuration_);
			attackCooldown_ = lv.value("attackCooldown", attackCooldown_);
			maxLayers_ = lv.value("maxLayers", maxLayers_);
			baseDamage_ = damage_;
			return;
		}
	}

	damage_ += levelUpCfg_.value("damage", 4);
	radius_ += levelUpCfg_.value("radius", 10.0f);
	areaDuration_ += levelUpCfg_.value("areaDuration", 0.2f);

	float cdDelta = levelUpCfg_.value("cooldownDelta", -0.2f);
	float cdMin = levelUpCfg_.value("cooldownMin", 1.0f);
	attackCooldown_ = std::max(cdMin, attackCooldown_ + cdDelta);

	int everyN = levelUpCfg_.value("maxLayersEveryN", 2);
	int maxCap = levelUpCfg_.value("maxLayersMax", 6);
	if (level_ % everyN == 0 && maxLayers_ < maxCap)
		maxLayers_++;

	baseDamage_ = damage_;
}

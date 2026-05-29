#include "SeaHeartWeapon.h"
#include "../Monster/Monster.h"
#include "../Projectile/BubbleProjectile.h"
#include "../Config/ConfigLoader.h"

SeaHeartWeapon::SeaHeartWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/seaheart.json");
	name_ = cfg.value("name", "Sea Heart");
	textureId_ = cfg.value("textureId", "seaheart");
	baseDamage_ = cfg.value("baseDamage", 2);
	damage_ = cfg.value("damage", 2);
	range_ = cfg.value("range", 400.0f);
	attackCooldown_ = cfg.value("attackCooldown", 3.0f);
	currentCooldown_ = 0.0f;
	dropletCount_ = cfg.value("dropletCount", 1);
	bounceCount_ = cfg.value("bounceCount", 3);
	dropletSpeed_ = cfg.value("dropletSpeed", 600.0f);
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
}

void SeaHeartWeapon::update(float deltaTime)
{
	if (currentCooldown_ > 0.0f)
		currentCooldown_ -= deltaTime;
}

void SeaHeartWeapon::attack(const sf::Vector2f &playerPos,
							 const sf::Vector2f &attackDir,
							 std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)attackDir;
	if (!canAttack())
		return;

	static std::random_device rd;
	static std::mt19937 gen(rd());

	// 收集范围内的怪物索引
	std::vector<int> candidates;
	for (size_t i = 0; i < monsters.size(); ++i)
	{
		if (!monsters[i]->isAlive())
			continue;
		sf::Vector2f d = monsters[i]->getPosition() - playerPos;
		float dist = std::sqrt(d.x * d.x + d.y * d.y);
		if (dist <= range_)
			candidates.push_back((int)i);
	}

	if (candidates.empty())
		return;

	// 生成水滴
	for (int d = 0; d < dropletCount_; ++d)
	{
		int idx = candidates[gen() % candidates.size()];

		auto bubble = std::make_unique<BubbleProjectile>();
		bubble->setPosition(playerPos);
		bubble->setTargetPos(monsters[idx]->getPosition());
		bubble->setDamage(damage_);
		bubble->setSpeed(dropletSpeed_);
		bubble->setMaxBounces(bounceCount_);
		bubble->setFriendly(true);
		addProjectile(std::move(bubble));
	}

	resetCooldown();
}

void SeaHeartWeapon::levelUp()
{
	Weapon::levelUp();
	bounceCount_ += levelUpCfg_.value("bounceCount", 1);
	dropletSpeed_ += levelUpCfg_.value("dropletSpeed", 50.0f);

	int everyN = levelUpCfg_.value("dropletCountEveryN", 3);
	if (level_ % everyN == 0)
		dropletCount_ += 1;
}

#include "SeaHeartWeapon.h"
#include "../Monster/Monster.h"
#include "../Projectile/BubbleProjectile.h"

SeaHeartWeapon::SeaHeartWeapon()
{
	name_ = "Sea Heart";
	textureId_ = "seaheart";
	baseDamage_ = 2;
	damage_ = 2;
	range_ = 400.0f;
	attackCooldown_ = 3.0f;
	currentCooldown_ = 0.0f;
	dropletCount_ = 1;
	bounceCount_ = 3;
	dropletSpeed_ = 600.0f;
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
	damage_ += 2;
	bounceCount_ += 1;
	dropletSpeed_ += 50.0f;
	if (level_ % 3 == 0)
		dropletCount_ += 1;
	attackCooldown_ = std::max(1.0f, attackCooldown_ - 0.2f);
}

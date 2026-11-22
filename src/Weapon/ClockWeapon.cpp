#include "ClockWeapon.h"
#include "WeaponManager.h"
#include "../Monster/Monster.h"
#include "../Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"

ClockWeapon::ClockWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/clock.json");
	name_ = cfg.value("name", "Clock");
	textureId_ = cfg.value("textureId", "clock");
	baseDamage_ = cfg.value("baseDamage", 0);
	damage_ = cfg.value("damage", 0);
	attackCooldown_ = cfg.value("attackCooldown", 30.0f);
	range_ = cfg.value("range", 0.0f);
	level_ = 1;
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});
}

void ClockWeapon::attack(const sf::Vector2f &playerPos,
						  const sf::Vector2f &attackDir,
						  std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)playerPos;
	(void)attackDir;

	if (!canAttack())
		return;

	SoundManager::getInstance().playSFX("madeinheaven");

	if (firstTrigger_)
	{
		firstTrigger_ = false;
		SoundManager::getInstance().playBGM("assets/audio/bgm/heaven.mp3", true);
	}

	// 刷新所有武器冷却
	if (weaponManager_)
	{
		for (auto &w : weaponManager_->getWeapons())
		{
			w->clearCooldown();
		}
	}

	// 10级冻结所有怪物1秒
	if (level_ >= 10)
	{
		for (auto &m : monsters)
		{
			if (m->isAlive())
				m->freeze(2.0f);
		}
	}

	resetCooldown();
}

void ClockWeapon::levelUp()
{
	Weapon::levelUp();
	attackCooldown_ = std::max(10.0f, attackCooldown_ - 2.0f);
}

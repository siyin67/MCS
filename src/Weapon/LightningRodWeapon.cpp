#include "LightningRodWeapon.h"
#include "../Monster/Monster.h"
#include "../Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

LightningRodWeapon::LightningRodWeapon()
{
	auto cfg = loadJsonConfig("assets/config/weapon/lightingrod.json");
	name_ = cfg.value("name", "Lightning Rod");
	textureId_ = cfg.value("textureId", "lightingrod");
	baseDamage_ = cfg.value("baseDamage", 30);
	damage_ = cfg.value("damage", 30);
	attackCooldown_ = cfg.value("attackCooldown", 10.0f);
	range_ = cfg.value("range", 600.0f);
	level_ = 1;
	levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});

	chainCount_ = cfg.value("chainCount", 5);
	chainRadius_ = cfg.value("chainRadius", 300.0f);
	chainDamagePercent_ = cfg.value("chainDamagePercent", 0.25f);
	displayTimer_ = 0.0f;

	std::random_device rd;
	rng_ = std::mt19937(rd());
}

std::vector<sf::Vector2f> LightningRodWeapon::generatePoints(sf::Vector2f from, sf::Vector2f to)
{
	std::vector<sf::Vector2f> points;
	points.push_back(from);

	sf::Vector2f delta = to - from;
	float totalDist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	if (totalDist < 1.0f)
	{
		points.push_back(to);
		return points;
	}

	sf::Vector2f dir = delta / totalDist;
	sf::Vector2f perp(-dir.y, dir.x);

	int segments = std::max(3, (int)(totalDist / 40.0f));
	std::uniform_real_distribution<float> offsetDist(-totalDist * 0.15f, totalDist * 0.15f);

	for (int i = 1; i < segments; i++)
	{
		float t = (float)i / segments;
		sf::Vector2f base = from + delta * t;
		base += perp * offsetDist(rng_);
		points.push_back(base);
	}
	points.push_back(to);
	return points;
}

void LightningRodWeapon::update(float deltaTime)
{
	Weapon::update(deltaTime);

	if (displayTimer_ > 0.0f)
	{
		displayTimer_ -= deltaTime;
		if (displayTimer_ <= 0.0f)
			currentBolts_.clear();
	}
}

void LightningRodWeapon::attack(const sf::Vector2f &playerPos,
								const sf::Vector2f &attackDir,
								std::vector<std::unique_ptr<Monster>> &monsters)
{
	(void)attackDir;

	if (!canAttack())
		return;

	Monster *primary = nullptr;
	float primaryDist = range_ * range_;
	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;
		float dx = m->getPosition().x - playerPos.x;
		float dy = m->getPosition().y - playerPos.y;
		float d2 = dx * dx + dy * dy;
		if (d2 < primaryDist)
		{
			primaryDist = d2;
			primary = m.get();
		}
	}

	if (!primary)
		return;

	// 雷声三选一随机播放（素材包没有 lightning_bolt.impact，用 weather/thunder1-3 代替），音量 0.85
	static const std::vector<std::string> kThunderIds = {
		"lightning_strike_1", "lightning_strike_2", "lightning_strike_3"};
	SoundManager::getInstance().playSFXRandom(kThunderIds, 0.85f);

	currentBolts_.clear();

	// 主雷击
	int fullDamage = damage_;
	primary->takeDamage(fullDamage);
	{
		LightningBolt bolt;
		bolt.from = playerPos;
		bolt.to = primary->getPosition();
		bolt.points = generatePoints(bolt.from, bolt.to);
		currentBolts_.push_back(bolt);
	}

	// 连锁闪电 (等级 > 5)
	if (level_ > 5 && chainCount_ > 0)
	{
		int chainDamage = (int)(fullDamage * chainDamagePercent_);
		if (chainDamage < 1) chainDamage = 1;

		std::vector<Monster *> hitList;
		hitList.push_back(primary);

		sf::Vector2f chainFrom = primary->getPosition();

		for (int c = 0; c < chainCount_; c++)
		{
			Monster *nextTarget = nullptr;
			float bestDist = chainRadius_ * chainRadius_;

			for (auto &m : monsters)
			{
				if (!m->isAlive())
					continue;
				if (std::find(hitList.begin(), hitList.end(), m.get()) != hitList.end())
					continue;

				float dx = m->getPosition().x - chainFrom.x;
				float dy = m->getPosition().y - chainFrom.y;
				float d2 = dx * dx + dy * dy;
				if (d2 < bestDist)
				{
					bestDist = d2;
					nextTarget = m.get();
				}
			}

			if (!nextTarget)
				break;

			nextTarget->takeDamage(chainDamage);
			hitList.push_back(nextTarget);

			LightningBolt bolt;
			bolt.from = chainFrom;
			bolt.to = nextTarget->getPosition();
			bolt.points = generatePoints(bolt.from, bolt.to);
			currentBolts_.push_back(bolt);

			chainFrom = nextTarget->getPosition();
		}
	}

	displayTimer_ = 0.3f;
	resetCooldown();
}

void LightningRodWeapon::render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const
{
	(void)playerPos;

	if (displayTimer_ <= 0.0f || currentBolts_.empty())
		return;

	for (const auto &bolt : currentBolts_)
	{
		if (bolt.points.size() < 2)
			continue;

		// Layer 1: thick glow (semi-transparent blue) — 8 copies offset
		for (int i = -4; i <= 4; i++)
		{
			sf::VertexArray layer(sf::PrimitiveType::LineStrip);
			for (const auto &p : bolt.points)
				layer.append({p + sf::Vector2f((float)i, (float)i), sf::Color(100, 150, 255, 100)});
			window.draw(layer);
		}

		// Layer 2: medium bright (light blue) — 4 copies offset
		for (int i = -2; i <= 2; i++)
		{
			sf::VertexArray layer(sf::PrimitiveType::LineStrip);
			for (const auto &p : bolt.points)
				layer.append({p + sf::Vector2f((float)i, (float)i), sf::Color(150, 200, 255, 200)});
			window.draw(layer);
		}

		// Layer 3: thin core (white)
		{
			sf::VertexArray core(sf::PrimitiveType::LineStrip);
			for (const auto &p : bolt.points)
				core.append({p, sf::Color::White});
			window.draw(core);
		}
	}
}

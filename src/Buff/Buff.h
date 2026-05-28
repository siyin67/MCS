#pragma once
#include <vector>
#include <algorithm>

enum class BuffType
{
	SpeedUp,
	SpeedDown,
	AttackUp,
	AttackDown,
	Invincible,
	Heal,
	Burn,
	Poison,
	Wither
};

struct Buff
{
	BuffType type;
	float value;
	float duration;
	float tickTimer;
};

class BuffManager
{
private:
	std::vector<Buff> buffs_;

	static bool isTickType(BuffType t)
	{
		return t == BuffType::Heal || t == BuffType::Burn || t == BuffType::Poison || t == BuffType::Wither;
	}

public:
	void add(BuffType type, float value, float duration)
	{
		for (auto &b : buffs_)
		{
			if (b.type == type)
			{
				b.value = value;
				b.duration = duration;
				return;
			}
		}
		buffs_.push_back({type, value, duration, 0.0f});
	}

	void update(float deltaTime, int &healthRef)
	{
		for (auto &b : buffs_)
			b.duration -= deltaTime;

		for (auto &b : buffs_)
		{
			if (isTickType(b.type))
			{
				b.tickTimer += deltaTime;
				if (b.tickTimer >= 1.0f)
				{
					b.tickTimer -= 1.0f;
					if (b.type == BuffType::Heal)
						healthRef += static_cast<int>(b.value);
					else
						healthRef -= static_cast<int>(b.value);
				}
			}
		}

		buffs_.erase(
			std::remove_if(buffs_.begin(), buffs_.end(),
						   [](const Buff &b) { return b.duration <= 0.0f; }),
			buffs_.end());
	}

	std::vector<std::pair<BuffType, float>> getActiveBuffs() const
	{
		std::vector<std::pair<BuffType, float>> result;
		for (const auto &b : buffs_)
		{
			if (b.duration > 0.0f)
				result.push_back({b.type, b.duration});
		}
		return result;
	}

	void clear() { buffs_.clear(); }

	float getSpeedMultiplier() const
	{
		float m = 1.0f;
		for (const auto &b : buffs_)
			if (b.type == BuffType::SpeedUp || b.type == BuffType::SpeedDown)
				m *= b.value;
		return m;
	}

	float getDamageMultiplier() const
	{
		float m = 1.0f;
		for (const auto &b : buffs_)
			if (b.type == BuffType::AttackUp || b.type == BuffType::AttackDown)
				m *= b.value;
		return m;
	}

	bool hasInvincible() const
	{
		for (const auto &b : buffs_)
			if (b.type == BuffType::Invincible)
				return true;
		return false;
	}
};

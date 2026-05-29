#include "Weapon.h"
#include "../Monster/Monster.h"

Weapon::Weapon()
    : baseDamage_(10), damage_(10), level_(1), attackCooldown_(0.5f), currentCooldown_(0.0f), range_(50.0f)
{
}

void Weapon::update(float deltaTime)
{
    if (currentCooldown_ > 0.0f)
    {
        currentCooldown_ -= deltaTime;
        if (currentCooldown_ < 0.0f)
            currentCooldown_ = 0.0f;
    }
}

void Weapon::dealDamage(Monster &monster)
{
    monster.takeDamage(damage_);
}

void Weapon::levelUp()
{
    ++level_;
    if (!levelUpCfg_.empty())
    {
        float cdDelta = levelUpCfg_.value("cooldownDelta", -0.05f);
        float cdMin = levelUpCfg_.value("cooldownMin", 0.3f);
        attackCooldown_ = std::max(cdMin, attackCooldown_ + cdDelta);
        range_ += levelUpCfg_.value("range", 10.0f);

        int dmg = levelUpCfg_.value("damage", 0);
        if (dmg == 0)
            damage_ = baseDamage_ * level_;
        else
            damage_ += dmg;
    }
    else
    {
        damage_ = baseDamage_ * level_;
        attackCooldown_ = std::max(0.3f, attackCooldown_ - 0.05f);
        range_ += 10.0f;
    }
}

bool Weapon::canAttack() const
{
    return currentCooldown_ <= 0.0f;
}

void Weapon::resetCooldown()
{
    currentCooldown_ = attackCooldown_;
}
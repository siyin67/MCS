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
    damage_ = baseDamage_ * level_;
    attackCooldown_ = std::max(0.3f, attackCooldown_ - 0.05f);
    range_ += 10.0f;
}

bool Weapon::canAttack() const
{
    return currentCooldown_ <= 0.0f;
}

void Weapon::resetCooldown()
{
    currentCooldown_ = attackCooldown_;
}
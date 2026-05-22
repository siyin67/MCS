#include "weapon.h"
#include "../Monster/Monster.h"

Weapon::Weapon()
    : baseDamage_(10)
    , damage_(10)
    , level_(1)
    , attackCooldown_(0.5f)
    , currentCooldown_(0.0f)
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

void Weapon::dealDamage(Monster& monster)
{
    monster.takeDamage(damage_);
    resetCooldown();
}

void Weapon::levelUp()
{
    ++level_;
    damage_ = baseDamage_ * level_;
}

bool Weapon::canAttack() const
{
    return currentCooldown_ <= 0.0f;
}

void Weapon::resetCooldown()
{
    currentCooldown_ = attackCooldown_;
}

int Weapon::getDamage() const { return damage_; }
int Weapon::getBaseDamage() const { return baseDamage_; }
int Weapon::getLevel() const { return level_; }
float Weapon::getAttackCooldown() const { return attackCooldown_; }
float Weapon::getCurrentCooldown() const { return currentCooldown_; }

#pragma once

#include <string>

class Monster;

class Weapon
{
protected:
    int baseDamage_;
    int damage_;
    int level_;
    float attackCooldown_;
    float currentCooldown_;
    std::string name_;
    std::string textureId_;

public:
    Weapon();
    virtual ~Weapon() = default;

    virtual void update(float deltaTime);
    virtual void dealDamage(Monster& monster);
    virtual void levelUp();

    bool canAttack() const;
    void resetCooldown();

    int getDamage() const;
    int getBaseDamage() const;
    int getLevel() const;
    float getAttackCooldown() const;
    float getCurrentCooldown() const;
    const std::string& getName() const;
    const std::string& getTextureId() const;

    void setName(const std::string& name) { name_ = name; }
    void setTextureId(const std::string& id) { textureId_ = id; }
};

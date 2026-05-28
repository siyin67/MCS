#pragma once
#include "Projectile.h"
#include <functional>

class MonsterProjectile : public Projectile
{
public:
    enum class AreaType { None, Wither, DragonBreath };

private:
    sf::Vector2f direction_;
    float speed_;
    int areaDamage_;
    float areaRadius_;
    float areaDuration_;
    AreaType areaType_;
    std::function<void(sf::Vector2f, float, float, int, AreaType)> onSpawnArea_;

public:
    MonsterProjectile();

    void setDirection(const sf::Vector2f &dir) { direction_ = dir; }
    void setSpeed(float speed) { speed_ = speed; }
    void setAreaParams(AreaType type, float radius, float duration, int damage)
    {
        areaType_ = type;
        areaRadius_ = radius;
        areaDuration_ = duration;
        areaDamage_ = damage;
    }
    AreaType getAreaType() const { return areaType_; }
    float getAreaRadius() const { return areaRadius_; }
    float getAreaDuration() const { return areaDuration_; }
    int getAreaDamage() const { return areaDamage_; }
    void setOnSpawnArea(std::function<void(sf::Vector2f, float, float, int, AreaType)> cb) { onSpawnArea_ = std::move(cb); }

    void update(float deltaTime) override;
    void onHit(Monster &monster) override;
};

#pragma once
#include "Projectile.h"

class Arrow : public Projectile
{
public:
    Arrow();
    void onHit(Monster &monster) override;
    void setVelocity(const sf::Vector2f &vel);
};
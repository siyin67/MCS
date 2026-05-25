#pragma once
#include "Monster.h"

class Zombie : public Monster
{
    std::string id_;

public:
    Zombie();
    void playHitSound() override;
    virtual void playDeathSound() override;
};
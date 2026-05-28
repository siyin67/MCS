#pragma once
#include "Monster.h"

class Ghast : public Monster
{
public:
	Ghast();
	void playHitSound() override;
	void playDeathSound() override;
};

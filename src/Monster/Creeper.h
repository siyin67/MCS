#pragma once
#include "Monster.h"

class Creeper : public Monster
{
public:
	Creeper();
	void playHitSound() override;
	void playDeathSound() override;
};

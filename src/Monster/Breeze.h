#pragma once
#include "Monster.h"

class Breeze : public Monster
{
public:
	Breeze();
	void playHitSound() override;
	void playDeathSound() override;
};

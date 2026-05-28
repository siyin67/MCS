#pragma once
#include "Monster.h"

class Wither : public Monster
{
public:
	Wither();
	void playHitSound() override;
	void playDeathSound() override;
};

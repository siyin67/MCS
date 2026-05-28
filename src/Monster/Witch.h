#pragma once
#include "Monster.h"

class Witch : public Monster
{
public:
	Witch();
	void playHitSound() override;
	void playDeathSound() override;
};

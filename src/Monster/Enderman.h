#pragma once
#include "Monster.h"

class Enderman : public Monster
{
public:
	Enderman();
	void playHitSound() override;
	void playDeathSound() override;
};

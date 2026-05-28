#pragma once
#include "Monster.h"

class Drowned : public Monster
{
public:
	Drowned();
	void playHitSound() override;
	void playDeathSound() override;
};

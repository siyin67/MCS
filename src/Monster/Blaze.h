#pragma once
#include "Monster.h"

class Blaze : public Monster
{
public:
	Blaze();
	void playHitSound() override;
	void playDeathSound() override;
};

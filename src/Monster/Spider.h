#pragma once
#include "Monster.h"

class Spider : public Monster
{
public:
	Spider();
	void playHitSound() override;
	void playDeathSound() override;
};

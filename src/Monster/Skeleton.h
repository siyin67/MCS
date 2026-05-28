#pragma once
#include "Monster.h"

class Skeleton : public Monster
{
public:
	Skeleton();
	void playHitSound() override;
	void playDeathSound() override;
};

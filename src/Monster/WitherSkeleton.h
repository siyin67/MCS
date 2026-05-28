#pragma once
#include "Monster.h"

class WitherSkeleton : public Monster
{
public:
	WitherSkeleton();
	void playHitSound() override;
	void playDeathSound() override;
};

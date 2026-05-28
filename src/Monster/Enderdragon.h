#pragma once
#include "Monster.h"

class Enderdragon : public Monster
{
public:
	Enderdragon();
	void playHitSound() override;
	void playDeathSound() override;
};

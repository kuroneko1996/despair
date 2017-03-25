#pragma once
#include "Destructible.h"

class PlayerDestructible : public Destructible
{
public:
	PlayerDestructible(float maxHp, float defense, const char *corpseName);
	void die(Actor *owner) override;
};


#pragma once

#include "Destructible.h"


class MonsterDestructible : public Destructible
{
public:
	MonsterDestructible(float maxHp, float defense, const char *corpseName, int xp);
	void die(Actor *owner) override;
};


#pragma once
#include "Ai.h"
class MonsterAi : public Ai
{
public:
	static const int TRACKING_TURNS;

	void update(Actor *owner);
protected:
	int moveCount = 0;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};


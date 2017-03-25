#pragma once
#include "Ai.h"
class PlayerAi :
	public Ai
{
public:
	static const int LEVEL_UP_BASE;
	static const int LEVEL_UP_FACTOR;

	int xpLevel = 1;

	PlayerAi();
	void update(Actor *owner) override;
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	void handleActionKey(Actor *owner, int key);
	int getNextLevelXp();
protected:
	Actor* choseFromInventory(Actor *owner);
};


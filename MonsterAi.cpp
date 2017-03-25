#include "MonsterAi.h"
#include "Actor.h"
#include "Engine.h"
#include <math.h>

const int MonsterAi::TRACKING_TURNS = 3;

void MonsterAi::update(Actor * owner)
{
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}
	Actor *target = engine.player;
	if (engine.map->isInFov(owner->x, owner->y)) {
		moveCount = TRACKING_TURNS;
	} else {
		moveCount--;
	}
	if (moveCount > 0) {
		moveOrAttack(owner, target->x, target->y);
	}
}

void MonsterAi::moveOrAttack(Actor * owner, int targetx, int targety)
{
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);
	float distance = sqrtf((float)(dx*dx + dy*dy));
	Actor *target = engine.player;

	if (distance >= 2) { // out of melee range trying to move closer
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));

		if (engine.map->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		} else if (engine.map->canWalk(owner->x + stepdx, owner->y)) { 
			// trying to move 1 step horizontally
			owner->x += stepdx;
		} else if (engine.map->canWalk(owner->x, owner->y + stepdy)) {
			// trying to move 1 step vertically
			owner->y += stepdy;
		}
	}
	else if (owner->attacker) { // in range
		owner->attacker->attack(owner, target);
	}
}

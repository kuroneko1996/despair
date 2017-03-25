#include <stdio.h>
#include "MonsterDestructible.h"
#include "Actor.h"
#include "Engine.h"

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char * corpseName, int xp) : Destructible(maxHp, defense, corpseName, xp)
{
}

void MonsterDestructible::die(Actor * owner)
{
	engine.gui->message(TCODColor::lightGrey, "%s is dead. You gain %d xp", owner->name, xp);
	engine.player->destructible->xp += xp;
	Destructible::die(owner);
}

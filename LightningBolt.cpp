#include "LightningBolt.h"
#include "Engine.h"



LightningBolt::LightningBolt(float range, float damage) : range(range), damage(damage)
{
}

bool LightningBolt::use(Actor * owner, Actor * wearer)
{
	Actor *closestMonster = engine.getClosestMonster(wearer->x, wearer->y, range);
	if (!closestMonster) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enough to strike.");
		return false;
	}
	engine.gui->message(TCODColor::lightBlue,
		"A lighting bolt strikes the %s with a loud thunder!\n"
		"The damage is %g hit points.",
		closestMonster->name, damage);

	closestMonster->destructible->takeDamage(closestMonster, damage);
	return Pickable::use(owner, wearer);
}

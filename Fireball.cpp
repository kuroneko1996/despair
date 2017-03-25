#include "Fireball.h"
#include "Engine.h"

Fireball::Fireball(float range, float damage) : LightningBolt(range, damage)
{
}

bool Fireball::use(Actor * owner, Actor * wearer)
{
	engine.gui->message(TCODColor::cyan, "Left click a target for the fireball,\n or right click to cancel");
	int x, y;
	if (!engine.pickAtile(&x, &y)) {
		return false;
	}
	// burn everything in range
	engine.gui->message(TCODColor::orange, "The fireball explodes, burning everything with %g tiles!", range);
	for (Actor **iterator = engine.actors.begin();
		iterator < engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->getDistance(x, y) <= range) {
			engine.gui->message(TCODColor::orange, "The %s gets burned for %g hit points.",
				actor->name, damage);
			actor->destructible->takeDamage(actor, damage);
		}
	}
	return Pickable::use(owner, wearer);
}

#include "Pickable.h"

#include "Actor.h"
#include "Engine.h"

Pickable::Pickable()
{
}


Pickable::~Pickable()
{
}

// owner = pickable owner, it's an item actually
bool Pickable::pick(Actor * owner, Actor * wearer)
{
	if (wearer->container && wearer->container->add(owner)) {
		engine.actors.remove(owner);
		return true;
	}
	return false;
}

void Pickable::drop(Actor * owner, Actor * wearer)
{
	if (wearer->container) {
		wearer->container->remove(owner);
		engine.actors.push(owner);
		owner->x = wearer->x;
		owner->y = wearer->y;
		engine.gui->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name, owner->name);
	}
}

bool Pickable::use(Actor * owner, Actor * wearer)
{
	if (wearer->container) {
		wearer->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}

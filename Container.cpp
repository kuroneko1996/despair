#include "Container.h"



Container::Container(int maxSize) : maxSize(maxSize)
{
}


Container::~Container()
{
	inventory.clearAndDelete();
}

bool Container::add(Actor * actor)
{
	if (maxSize > 0 && inventory.size() >= maxSize) {
		// inventory full
		return false;
	}
	inventory.push(actor);
	return true;
}

void Container::remove(Actor * actor)
{
	inventory.remove(actor);
}

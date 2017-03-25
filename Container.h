#pragma once

#include "libtcod.hpp"
class Actor;

class Container
{
public:
	int maxSize; // maximum number of actors. 0=unlimited
	TCODList<Actor *> inventory;

	Container(int maxSize);
	~Container();

	bool add(Actor *actor);
	void remove(Actor *actor);
};


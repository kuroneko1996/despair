#pragma once
#include "libtcod.hpp"
#include "Destructible.h"
#include "Attacker.h"
#include "Ai.h"
#include "Pickable.h"
#include "Container.h"

class Actor
{
public:
	enum Types
	{
		NONE, PLAYER, MONSTER, STAIRS
	};

	Types type;

	int x, y;
	int ch; // ascii code
	const char *name;
	TCODColor col;
	bool blocks; // does it block a path?
	bool alwaysVisible = false; // outside fov
	Attacker *attacker; // something can attack
	Destructible *destructible; // something has life
	Ai *ai; // something self updating
	Pickable *pickable; // something that can be picked
	Container *container; // something that can contain actors

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);
	~Actor();
	void update();
	void render() const;
	void clear();
	float getDistance(int cx, int cy) const;
};


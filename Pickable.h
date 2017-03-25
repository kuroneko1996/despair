#pragma once

class Actor;

class Pickable
{
public:
	Pickable();
	virtual ~Pickable();

	bool pick(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
};


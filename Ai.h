#pragma once

class Actor;

class Ai
{
public:
	virtual void update(Actor *owner) = 0; // pure virtual
	virtual ~Ai() {};
};


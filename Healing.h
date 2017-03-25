#pragma once
#include "Pickable.h"
class Actor;

class Healing : public Pickable
{
public:
	float amount; // how many hp it heals
	Healing(float amount);
	bool use(Actor *owner, Actor *wearer) override;
};


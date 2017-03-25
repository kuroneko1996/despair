#pragma once
#include "LightningBolt.h"
class Fireball : public LightningBolt
{
public:
	Fireball(float range, float damage);
	bool use(Actor *owner, Actor *wearer) override;
};


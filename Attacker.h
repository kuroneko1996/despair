#pragma once

class Actor;

class Attacker
{
public:
	float base_power; // hit points given

	Attacker(float power);
	void attack(Actor *owner, Actor *target);
	float power(Actor * owner);
};


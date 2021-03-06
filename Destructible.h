#pragma once

class Actor;

class Destructible
{
public:
	float maxHp;
	float hp;
	float base_defense;
	const char *corpseName;
	int xp; // xp gained when killing others destructible

	Destructible(float maxHp, float defense, const char *corpseName, int xp);
	virtual ~Destructible() {};
	inline bool isDead() { return hp <= 0; };
	float takeDamage(Actor *owner, float damage);
	float heal(float amount);
	float defense(Actor * owner);
	virtual void die(Actor *owner);
};


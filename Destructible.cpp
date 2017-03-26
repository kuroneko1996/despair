#include "Engine.h"
#include "Destructible.h"
#include "libtcod.hpp"

Destructible::Destructible(float maxHp, float defense, const char * corpseName, int xp) :
	maxHp(maxHp),hp(maxHp),base_defense(defense),corpseName(corpseName),xp(xp)
{
}

float Destructible::takeDamage(Actor * owner, float damage)
{
	damage -= defense(owner);
	if (damage > 0) {
		hp -= damage;
		if (hp <= 0) {
			die(owner);
		}
	}
	else {
		damage = 0;
	}
	return damage;
}

float Destructible::heal(float amount)
{
	hp += amount;
	if (hp > maxHp) {
		amount -= hp - maxHp;
		hp = maxHp;
	}
	return amount;
}

float Destructible::defense(Actor * owner)
{
	auto items = owner->getAllEquipped();
	float result_defense = base_defense;

	for (auto it = items.begin(); it < items.end(); it++) {
		Equipment *equipment = *it;
		result_defense += equipment->defense_bonus;
	}
	return result_defense;
}

void Destructible::die(Actor * owner)
{
	owner->ch = '%';
	owner->col = TCODColor::darkRed;
	owner->name = corpseName;
	owner->blocks = false;
	engine.sendToBack(owner);
}

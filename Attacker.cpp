#include "Attacker.h"
#include "Actor.h"
#include <stdio.h>
#include "Engine.h"

Attacker::Attacker(float power) : power(power)
{
}

void Attacker::attack(Actor * owner, Actor * target)
{
	if (target->destructible && !target->destructible->isDead()) {
		float attack_hit_points = power - target->destructible->defense;
		if (attack_hit_points > 0) {
			engine.gui->message(target == engine.player ? TCODColor::red : TCODColor::lightGrey,
				"%s attacks %s for %g hit points.", owner->name, target->name, attack_hit_points);
		}
		else {
			engine.gui->message(TCODColor::lightGrey,"%s attacks %s but it has no effect.", owner->name, target->name);
		}
		target->destructible->takeDamage(target, power);
	}
	else {
		engine.gui->message(TCODColor::lightGrey,"%s attacks %s in vain.", owner->name, target->name);
	}
}

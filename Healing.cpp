#include "Healing.h"
#include "Actor.h"


Healing::Healing(float amount) : amount(amount)
{
}

bool Healing::use(Actor * owner, Actor * wearer)
{
	if (wearer->destructible) {
		float amountHealed = wearer->destructible->heal(amount);
		if (amountHealed > 0) {
			return Pickable::use(owner, wearer);
		}
	}
	return false;
}

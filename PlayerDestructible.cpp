#include <stdio.h>
#include "PlayerDestructible.h"
#include "Actor.h"
#include "Engine.h"

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char * corpseName) : Destructible(maxHp, defense, corpseName, 0)
{
}

void PlayerDestructible::die(Actor * owner)
{
	engine.gui->message(TCODColor::red, "You died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}


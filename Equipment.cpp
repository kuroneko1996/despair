#include "Equipment.h"
#include "Actor.h"
#include "Engine.h"
#include <typeinfo>

const char * Equipment::SlotStrings[4] = { "Left Hand", "Right Hand", "Head", "Torso" };

Equipment::Equipment()
{
}

Equipment::Equipment(Slot slot) : slot(slot)
{
}

bool Equipment::equip(Actor *owner) {
	if (!equipped) {
		engine.gui->message(TCODColor::grey, "You equipped %s to %s",
			owner->name, getSlotName());
		equipped = true;
		return true;
	}
	return false;
}

bool Equipment::dequip(Actor *owner) {
	if (equipped) {
		engine.gui->message(TCODColor::grey, "You dequipped %s from %s",
			owner->name, getSlotName());
		equipped = false;
		return true;
	}
	return false;
}

Equipment* Equipment::getAtSlot(Slot slot, Container const *container) {
	for (auto it = container->inventory.begin(); it < container->inventory.end(); it++) {
		Actor* actor = *it;
		if (!actor->pickable) continue;

		if (actor->equipment) {
			Equipment *equipment = actor->equipment;
			if( equipment->slot == slot ) {
				return equipment;
			}
		}
	}
	return nullptr;
}

bool Equipment::toggleEquip(Actor *owner, Actor *wearer) {
	if (!wearer->container) {
		return false;
	}

	if (!equipped) {
		Equipment* oldEquipment = getAtSlot(slot, wearer->container);
		if (oldEquipment) {
			oldEquipment->dequip(owner);
		}
		equip(owner);
	}
	else {
		dequip(owner);
	}

	return true;
}

const char * Equipment::getSlotName(int enumVal)
{
	if (enumVal == -1) {
		return SlotStrings[slot];
	}
	return SlotStrings[enumVal];
}

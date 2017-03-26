#pragma once
#include "Pickable.h"

class Container;

class Equipment
{
public:
	enum Slot {
		LeftHand, RightHand, Head, Torso
	};
	Slot slot;
	bool equipped = false;
	float power_bonus = 0;
	float defense_bonus = 0;
	float max_hp_bonus = 0;

	Equipment();
	Equipment::Equipment(Slot slot);
	bool toggleEquip(Actor *owner, Actor *wearer);
	bool equip(Actor *owner);
	bool dequip(Actor *owner);
	Equipment* getAtSlot(Slot slot, Container const *container);
	const char * Equipment::getSlotName(int enumVal = -1);
protected:
	static const char * SlotStrings[4];
};


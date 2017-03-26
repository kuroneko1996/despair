#pragma once
#include "Pickable.h"

class Container;

class Equipment : public Pickable
{
public:
	enum Slot {
		LeftHand, RightHand, Head, Torso
	};
	Slot slot;
	bool equipped = false;

	Equipment();
	Equipment::Equipment(Slot slot);
	bool use(Actor *owner, Actor *wearer) override;
	bool equip(Actor *owner);
	bool dequip(Actor *owner);
	Equipment* getAtSlot(Slot slot, Container const *container);
	const char * Equipment::getSlotName(int enumVal = -1);
protected:
	static const char * SlotStrings[4];
};


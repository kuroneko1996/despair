#include "libtcod.hpp"
#include "BspListener.h"

BspListener::BspListener(Map & map) : map(map), roomNum(0)
{
}

bool BspListener::visitNode(TCODBsp * node, void * userData)
{
	if (node->isLeaf()) {
		bool withActors = (bool)userData;
		int x, y, w, h;
		// dig a room
		w = map.rng->getInt(Map::ROOM_MIN_SIZE, node->w - 2);
		h = map.rng->getInt(Map::ROOM_MIN_SIZE, node->h - 2);
		x = map.rng->getInt(node->x + 1, node->x + node->w - w - 1);
		y = map.rng->getInt(node->y + 1, node->y + node->h - h - 1);
		map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1, withActors);

		if (roomNum != 0) {
			// dig a corridor from the last room
			map.dig(lastx, lasty, x + w / 2, lasty);
			map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
		}
		lastx = x + w / 2;
		lasty = y + h / 2;
		roomNum++;
	}
	return true;
}


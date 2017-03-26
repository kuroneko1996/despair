#include <stdio.h>
#include <math.h>
#include "Actor.h"
#include "Engine.h"
#include "Camera.h"


Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) : 
	x(x),y(y),ch(ch),name(name),col(col), blocks(true),
	attacker(nullptr),destructible(nullptr), ai(nullptr),pickable(nullptr),container(nullptr)
{
}

Actor::~Actor()
{
	if (attacker) delete attacker;
	if (destructible) delete destructible;
	if (ai) delete ai;
	if (pickable) delete pickable;
	if (container) delete container;
	if (equipment) delete equipment;
}

void Actor::update() {
	if (ai) ai->update(this);
}

void Actor::render() const{
	int screenx = x;
	int screeny = y;

	bool visible = Camera::getInstance().to_screen(x, y, screenx, screeny);

	if (visible) {
		TCODConsole::root->setChar(screenx, screeny, ch);
		TCODConsole::root->setCharForeground(screenx, screeny, col);
	}
}

void Actor::clear()
{
	int screenx = x;
	int screeny = y;
	bool visible = Camera::getInstance().to_screen(x, y, screenx, screeny);
	if (visible) {
		TCODConsole::root->putChar(screenx, screeny, ' ', TCOD_BKGND_NONE);
	}
}

float Actor::getDistance(int cx, int cy) const
{
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}

std::vector<Equipment*> Actor::getAllEquipped()
{
	std::vector<Equipment*> result;

	if (name == "Player") { // TODO change to type
		for (Actor **it = container->inventory.begin(); it != container->inventory.end(); it++) {
			Actor* item = *it;
			if (item->equipment && item->equipment->equipped) {
				result.push_back(item->equipment);
			}
		}
	}

	return result;
}

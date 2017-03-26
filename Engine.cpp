#include "libtcod.hpp"
#include "Actor.h"
#include "Engine.h"
#include "PlayerDestructible.h"
#include "PlayerAi.h"
#include "Camera.h"
#include <stdio.h>


Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight), player(nullptr), map(nullptr)
{
	TCODConsole::setCustomFont("terminal16x16_gs_tc.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
	TCODConsole::initRoot(80, 50, "Dungeons of Despair", false);
	gui = new Gui();

	TCODConsole::mapAsciiCodeToFont(S_DAGGER, 1, 5);
	TCODConsole::mapAsciiCodeToFont(S_SWORD, 2, 5);
	TCODConsole::mapAsciiCodeToFont(S_SHIELD, 3, 5);
	TCODConsole::mapAsciiCodeToFont(S_POTION, 4, 5);
	TCODConsole::mapAsciiCodeToFont(S_SCROLL, 5, 5);
	TCODConsole::mapAsciiCodeToFont(S_ORC, 7, 5);
	TCODConsole::mapAsciiCodeToFont(S_FLOOR, 8, 5);
	TCODConsole::mapAsciiCodeToFont(S_WALL, 9, 5);
	TCODConsole::mapAsciiCodeToFont(S_STAIRS, 10, 5);
}

void Engine::init()
{
	createPlayer();
	createMap();
	gui->message(TCODColor::red,
		"Welcome stranger!\nPrepare to perish in the Dungeons of Despair.");
	gameStatus = STARTUP;
}

void Engine::load()
{
	gui->menu.clear();
	gui->menu.addItem(Menu::NEW_GAME, "New Game");
	if (TCODSystem::fileExists("game.save")) {
		engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
	}
	engine.gui->menu.addItem(Menu::EXIT, "Exit");
	Menu::MenuItemCode menuItem = gui->menu.pick();
	if (menuItem == Menu::EXIT || menuItem == Menu::NONE) {
		// exit or window closed
		exit(0);
	}
	else if (menuItem == Menu::NEW_GAME) {
		term();
		init();
	}
	else if (menuItem == Menu::CONTINUE) {

	}
}

void Engine::save()
{
}

void Engine::term()
{
	if (map) delete map;
	actors.clearAndDelete();
	gui->clear();
}

void Engine::nextLevel()
{
	++level;

	gui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp / 2);
	gui->message(TCODColor::red, "After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

	delete map;

	// delete all actors but player
	for (Actor **it = actors.begin(); it != actors.end(); it++) {
		Actor *actor = *it;
		if (actor != player) {
			delete actor;
			it = actors.remove(it);
		}
	}

	// create a new map
	map = new Map(100, 50);
	map->init(true);
	gameStatus = STARTUP;
}


Engine::~Engine()
{
	term();
	delete gui;
}

void Engine::update(bool ignoreInput)
{
	if (gameStatus == STARTUP) map->computeFov(player->x, player->y, fovRadius);
	gameStatus = IDLE;

	if (!ignoreInput) {
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse, true);
		if (lastKey.vk == TCODK_ESCAPE) {
			
		}
	}
	player->update();

	if (gameStatus == NEW_TURN) {
		for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
			Actor *actor = *iterator;
			if (actor != player) {
				actor->update();
			}
		}
	}
}

void Engine::render()
{
	TCODConsole::root->clear();

	Camera::getInstance().move(player->x, player->y, map->width, map->height);

	//if (gameStatus == NEW_TURN) {
		map->render();
	//}
	
	// render actors except player
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		bool visible = actor->alwaysVisible && map->isExplored(actor->x, actor->y);
		if (actor != player && (visible || map->isInFov(actor->x, actor->y)) ) {
			actor->render();
		}
	}
	player->render();
	gui->render();
}

void Engine::sendToBack(Actor * actor)
{
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

void Engine::createPlayer() {
	player = new Actor(40, 25, '@', "Player", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
}

void Engine::createMap() {
	map = new Map(100, 50);
	map->init(true);
}

Actor * Engine::getClosestMonster(int x, int y, float range) const
{
	Actor *closest = nullptr;
	float bestDistance = 1E6f;

	for (Actor **iterator = actors.begin(); iterator < actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor != player && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}

	return closest;
}

bool Engine::pickAtile(int *x, int *y, float maxRange)
{
	while (!TCODConsole::isWindowClosed()) {
		render();

		// highlight the possible range
		for (int cx = 0; cx < map->width; cx++) {
			for (int cy = 0; cy < map->height; cy++) {
				if (map->isInFov(cx, cy) 
					&& (maxRange == 0 || player->getDistance(cx,cy) <= maxRange) ) {
					int screenx = 0;
					int screeny = 0;
					if (Camera::getInstance().to_screen(cx, cy, screenx, screeny)) {
						//TCODColor col = TCODConsole::root->getCharBackground(screenx, screeny);
						//col = col * 1.2f;
						TCODConsole::root->setCharBackground(screenx, screeny, TCODColor::lighterOrange);
					}
				}
			}
		}

		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);

		int mapx = 0;
		int mapy = 0;
		Camera::getInstance().to_map(mouse.cx, mouse.cy, mapx, mapy);
		if (map->isInFov(mapx, mapy)
			&& (maxRange == 0 || player->getDistance(mapx, mapy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);

			if (mouse.lbutton_pressed) {
				*x = mapx;
				*y = mapy;
				return true;
			}
		}

		// if player pressed right key or anykey we exit
		if (mouse.rbutton_pressed || lastKey.vk == TCODK_ESCAPE) {
			return false;
		}

		TCODConsole::flush();
	}
	return false;
}


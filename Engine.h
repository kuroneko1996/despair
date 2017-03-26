#pragma once
#include "Actor.h"
#include "Map.h"
#include "Gui.h"

class Engine
{
public:
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;

	enum SpriteMap {
		S_DAGGER = 201,
		S_SWORD = 202,
		S_SHIELD = 203,
		S_POTION = 204,
		S_SCROLL = 205,
		S_ORC = 207,
		S_FLOOR = 208,
		S_WALL = 209,
		S_STAIRS = 210,
	} spriteMap;

	TCODList<Actor *> actors;
	Actor *player;
	Map *map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui *gui;

	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	int level = 1;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update(bool ignoreInput=false);
	void render();
	void sendToBack(Actor *actor);
	void createPlayer();
	void createMap();
	Actor* getClosestMonster(int x, int y, float range) const;
	bool pickAtile(int *x, int *y, float maxRange = 0.0f); // 0 - means infinity
	void init();
	void load();
	void save();
	void term();
	void nextLevel();
private:
	bool computeFov;
};

extern Engine engine;

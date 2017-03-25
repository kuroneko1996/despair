#include "libtcod.hpp"
#include "Map.h"
#include "BspListener.h"
#include "Actor.h"
#include "Engine.h"
#include "MonsterDestructible.h"
#include "MonsterAi.h"
#include "Healing.h"
#include "LightningBolt.h"
#include "Fireball.h"
#include "utility.h"
#include "Camera.h"

const int Map::ROOM_MAX_SIZE = 12;
const int Map::ROOM_MIN_SIZE = 6;
const int Map::MAX_ROOM_MONSTERS = 3;
const int Map::MAX_ROOM_ITEMS = 2;

Map::Map(int width, int height) : width(width),height(height)
{
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map::~Map() {
	delete map;
	delete[] tiles;
	rooms.clearAndDelete();
}

void Map::init(bool withActors)
{
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);

	tiles = new Tile[width * height];
	map = new TCODMap(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, (void *)withActors);

	Room* lastRoom = rooms.get(rooms.size() - 1);
	addStairs((lastRoom->x1 + lastRoom->x2)/2, (lastRoom->y1 + lastRoom->y2) / 2);
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
}

bool Map::isInFov(int x, int y) const
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	if (map->isInFov(x, y)) {
		tiles[x + y*width].explored = true;
		return true;
	}
	return false;
}

bool Map::isExplored(int x, int y) const
{
	return tiles[x + y*width].explored;
}

void Map::computeFov(int playerx, int playery, int fovRadius)
{
	map->computeFov(playerx, playery, fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2)
{
	if (x2 < x1) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tilex = x1; tilex <= x2; tilex++)
	{
		for (int tiley = y1; tiley <= y2; tiley++)
		{
			map->setProperties(tilex, tiley, true, true);
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors)
{
	dig(x1, y1, x2, y2);
	if (!withActors) {
		return;
	}

	if (first)
	{
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
	}
	else {
		// place objects/monsters/npcs
		TCODRandom *rng = TCODRandom::getInstance();
		int monster_counter = rng->getInt(0, MAX_ROOM_MONSTERS);
		while (monster_counter > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addMonster(x, y);
			}
			monster_counter--;
		}

		// add items
		int itemsNumber = rng->getInt(0, MAX_ROOM_ITEMS);
		while (itemsNumber > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addItem(x, y);
			}
			itemsNumber--;
		}
	}

	Room *room = new Room(x1, y1, x2, y2);
	rooms.push(room);
}

void Map::render() const {
	static const TCODColor darkWall { 0, 0, 100 };
	static const TCODColor darkGround { 50, 50, 150 };
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (isInFov(x, y)) {
				/*TCODConsole::root->setCharBackground(x, y,
					isWall(x, y) ? lightWall : lightGround);*/
				TCODConsole::root->setDefaultForeground(TCODColor::white * 0.9f);
			}
			else {
				TCODConsole::root->setDefaultForeground(TCODColor::white * 0.3f);
				/*TCODConsole::root->setCharBackground(x, y,
					isWall(x, y) ? darkWall : darkGround);*/
			}

			if (isExplored(x, y)) {
				int screenx = x;
				int screeny = y;
				bool visible = Camera::getInstance().to_screen(x, y, screenx, screeny);
				if (isWall(x, y)) {
					if (visible) {
						TCODConsole::root->putChar(screenx, screeny, Engine::S_WALL);
					}
				}
				else {
					if (visible) {
						TCODConsole::root->putChar(screenx, screeny, Engine::S_FLOOR);
					}
				}
			}
		}
	}
}

bool Map::canWalk(int x, int y) const
{
	if (isWall(x, y)) {
		return false;
	}
	for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

void Map::addMonster(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	std::vector<int> chances;
	chances.push_back(80); // orc
	chances.push_back(20); // troll
	int selected = utility::get_random(chances);

	if (selected == 0) {
		//create an orc
		Actor *orc = new Actor(x, y, Engine::S_ORC, "orc", TCODColor::white);
		orc->destructible = new MonsterDestructible(10, 0, "dead orc", 35);
		orc->attacker = new Attacker(3);
		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	}
	else if (selected == 1) {
		//create a troll
		Actor *troll = new Actor(x, y, Engine::S_ORC, "troll", TCODColor::darkGreen);
		troll->destructible = new MonsterDestructible(16, 1, "troll carcass", 100);
		troll->attacker = new Attacker(4);
		troll->ai = new MonsterAi();
		engine.actors.push(troll);
	}
}

void Map::addItem(int x, int y)
{
	std::vector<int> chances;
	chances.push_back(70); // health
	chances.push_back(20); // lightning
	chances.push_back(15); // fireball
	int selected = utility::get_random(chances);

	if (selected == 0) {
		Actor *healthPotion = new Actor(x, y, Engine::S_POTION, "health potion", TCODColor::white);
		healthPotion->blocks = false;
		healthPotion->pickable = new Healing(4);
		healthPotion->alwaysVisible = true;
		engine.actors.push(healthPotion);
	}
	else if (selected == 1) {
		Actor *lightningScroll = new Actor(x, y, Engine::S_SCROLL, "scroll of lightning bolt", TCODColor::white);
		lightningScroll->blocks = false;
		lightningScroll->pickable = new LightningBolt(5, 20);
		lightningScroll->alwaysVisible = true;
		engine.actors.push(lightningScroll);
	}
	else if (selected == 2) {
		Actor *fireballScroll = new Actor(x, y, Engine::S_SCROLL, "scroll of fireball", TCODColor::white);
		fireballScroll->blocks = false;
		fireballScroll->pickable = new Fireball(3, 12);
		fireballScroll->alwaysVisible = true;
		engine.actors.push(fireballScroll);
	}
}

void Map::addStairs(int x, int y) {
	Actor *stairs = new Actor(x, y, Engine::S_STAIRS, "stairs", TCODColor::white);
	stairs->type = Actor::STAIRS;
	stairs->blocks = false;
	stairs->alwaysVisible = true;
	engine.actors.push(stairs);
}
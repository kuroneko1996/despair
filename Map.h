#pragma once

struct Tile {
	bool explored;
	Tile() : explored(false) {}
};

struct Room {
	int x1, y1, x2, y2;

	Room(int x1, int y1, int x2, int y2) : x1(x1),y1(y1),x2(x2),y2(y2) {}
};

class Map
{
public:
	static const int ROOM_MAX_SIZE;
	static const int ROOM_MIN_SIZE;
	static const int MAX_ROOM_MONSTERS;
	static const int MAX_ROOM_ITEMS;

	int width, height;
	
	Map(int width, int height);
	~Map();
	bool isWall(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov(int playerx, int playery, int fovRadius);
	void render() const;
	bool canWalk(int x, int y) const;

	void addMonster(int x, int y);
	void addItem(int x, int y);
	void addStairs(int x, int y);
	void init(bool withActors);
protected:
	Tile *tiles;
	TCODMap *map;
	long seed;
	TCODRandom *rng;
	friend class BspListener;
	TCODList<Room *> rooms;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool WithActors);
};



#pragma once
#include "Map.h"

class BspListener : public ITCODBspCallback
{
private:
	Map &map;
	int roomNum{ 0 };
	int lastx{ 0 }, lasty{ 0 };

public:
	BspListener(Map &map);

	virtual bool visitNode(TCODBsp * node, void * userData) override;
};
#include "libtcod.hpp"
#include "Actor.h"
#include "Map.h"
#include "Engine.h"
#include "utility.h"

Engine engine(80, 50);

int main(void) {
	TCODSystem::setFps(20);
	bool ignoreInput = true; // prevent waitForEvent call for first update

	engine.load();
	while (!TCODConsole::isWindowClosed()) {
		engine.update(ignoreInput);
		engine.render();
		TCODConsole::flush();
		ignoreInput = false;
	}
	engine.save();
	return 0;
}

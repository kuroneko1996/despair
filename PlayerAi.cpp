#include "PlayerAi.h"
#include "Actor.h"
#include "Engine.h"
#include "Equipment.h"
#include <cstdio>
#include <typeinfo>

const int PlayerAi::LEVEL_UP_BASE = 200;
const int PlayerAi::LEVEL_UP_FACTOR = 150;

PlayerAi::PlayerAi()
{
}

void PlayerAi::update(Actor *owner)
{
	int levelUpXp = getNextLevelXp();
	if (owner->destructible->xp >= levelUpXp) {
		xpLevel++;
		owner->destructible->xp -= levelUpXp;
		engine.gui->message(TCODColor::yellow, "Your battle skills grow stronger! You reached level %d", xpLevel);
		engine.gui->menu.clear();
		engine.gui->menu.addItem(Menu::CONSTITUTION, "Constitution (+20HP)");
		engine.gui->menu.addItem(Menu::STRENGTH, "Strength (+1 attack)");
		engine.gui->menu.addItem(Menu::AGILITY, "Agility (+1 defense)");
		Menu::MenuItemCode menuItem = engine.gui->menu.pick(Menu::PAUSE);

		switch (menuItem)
		{
		case Menu::CONSTITUTION :
			owner->destructible->maxHp += 20;
			owner->destructible->hp += 20;
			break;
		case Menu::STRENGTH :
			owner->attacker->base_power += 1;
			break;
		case Menu::AGILITY :
			owner->destructible->base_defense += 1;
			break;
		default:
			break;
		}
	}

	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	int dx = 0, dy = 0;

	switch (engine.lastKey.vk)
	{
	case TCODK_UP:
	case TCODK_KP8:
		dy -= 1;
		break;
	case TCODK_DOWN:
	case TCODK_KP2:
		dy += 1;
		break;
	case TCODK_LEFT:
	case TCODK_KP4:
		dx -= 1;
		break;
	case TCODK_RIGHT:
	case TCODK_KP6:
		dx += 1;
		break;
	case TCODK_KP7:
		dx -= 1;
		dy -= 1;
		break;
	case TCODK_KP1:
		dx -= 1;
		dy += 1;
		break;
	case TCODK_KP9:
		dx += 1;
		dy -= 1;
		break;
	case TCODK_KP3:
		dx += 1;
		dy += 1;
		break;
	case TCODK_KP5:
		engine.gameStatus = Engine::NEW_TURN;
		break;
	case TCODK_CHAR: handleActionKey(owner, engine.lastKey.c); break;
	default:
		break;
	}

	if (dx != 0 || dy != 0) {
		engine.gameStatus = Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x + dx, owner->y + dy)) {
			engine.map->computeFov(owner->x, owner->y, engine.fovRadius);
		}
	}
}

bool PlayerAi::moveOrAttack(Actor * owner, int targetx, int targety)
{
	if (engine.map->isWall(targetx, targety)) return false;
	for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->x == targetx && actor->y == targety
			&& actor->destructible && !actor->destructible->isDead())
		{
			owner->attacker->attack(owner, actor);
			return false;
		}
	}

	owner->x = targetx;
	owner->y = targety;
	return true;
}

void PlayerAi::handleActionKey(Actor * owner, int key)
{
	switch (key)
	{
		case 'g': // pickup item
		{
			bool found = false;
			for (Actor **iterator = engine.actors.begin();
				iterator < engine.actors.end(); iterator++) {
				Actor *actor = *iterator;
				if (actor->x == owner->x && actor->y == owner->y && actor->pickable) {
					if (actor->pickable->pick(actor, owner)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You pick the %s.", actor->name);
						break; // safe
					}
					else if (!found) {
						found = true;
						engine.gui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if (!found) {
				engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick.");
			}
			engine.gameStatus = Engine::NEW_TURN;
		}
		break;

		case 'i': 
		{
			// owner is player
			Actor *item = choseFromInventory(owner);
			if (item) {
				if (item->equipment) {
					item->equipment->toggleEquip(item, owner);
					engine.gameStatus = Engine::NEW_TURN;
				}
				else {
					item->pickable->use(item, owner);
					engine.gameStatus = Engine::NEW_TURN;
				}
			}
		} 
		break;

		case 'd':
		{
			Actor *item = choseFromInventory(owner);
			if (item) {
				item->pickable->drop(item, owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		}
		break;

		case 'h':
		case '?':
		case '/':
		{
			engine.gui->showHelp();
		}
		break;

		case '>':
		case '.':
		{
			bool stairsFound = false;
			for (Actor **iterator = engine.actors.begin();
				iterator < engine.actors.end(); iterator++) {
				Actor *actor = (*iterator);
				if (actor->type == Actor::STAIRS && engine.player->x == actor->x 
					&& engine.player->y == actor->y) {
					stairsFound = true;
					break;
				}
			}
			if (stairsFound) {
				engine.nextLevel();
			} else {
				engine.gui->message(TCODColor::lightGrey, "There is no stairs here.");
			}
		}
		break;

		default:
			break;
		}
}

int PlayerAi::getNextLevelXp()
{
	return LEVEL_UP_BASE + xpLevel*LEVEL_UP_FACTOR;
}

// draw inventory screen
Actor * PlayerAi::choseFromInventory(Actor * owner)
{
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor **it = owner->container->inventory.begin();
		it != owner->container->inventory.end(); it++) {
		Actor *item = *it;

		if (item->equipment) {
			Equipment *equipment = item->equipment;
			if (equipment->equipped) {
				con.print(2, y, "(%c) [%s]%s", shortcut, item->name, equipment->getSlotName());
			}
			else {
				con.print(2, y, "(%c) %s", shortcut, item->name);
			}
		}
		else {
			con.print(2, y, "(%c) %s", shortcut, item->name);
		}
		y++;
		shortcut++;
	}

	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, engine.screenWidth / 2 - INVENTORY_WIDTH / 2,
		engine.screenHeight / 2 - INVENTORY_HEIGHT / 2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR) { // only printable type
		int itemIndex = key.c - 'a'; // b - a = 1; (a) item1 (b) item2
		if (itemIndex >= 0 && itemIndex < owner->container->inventory.size()) {
			return owner->container->inventory.get(itemIndex);
		}
	}

	return nullptr;
}

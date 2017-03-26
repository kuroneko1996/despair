#include <stdio.h>
#include <stdarg.h>
#include "libtcod.hpp"
#include "Gui.h"
#include "PlayerAi.h"
#include "Engine.h"
#include "Camera.h"

const int Gui::PANEL_HEIGHT = 5;
const int Gui::BAR_WIDTH = 20;
const int Gui::MSG_X = BAR_WIDTH + 2;
const int Gui::MSG_HEIGHT = PANEL_HEIGHT - 1;
const int Menu::PAUSE_MENU_WIDTH = 30;
const int Menu::PAUSE_MENU_HEIGHT = 15;

Gui::Gui()
{
	con = new TCODConsole(engine.screenWidth, PANEL_HEIGHT);
}

Gui::~Gui()
{
	delete con;
	clear();
}


void Gui::clear()
{
	log.clearAndDelete();
}

void Gui::render()
{
	con->setDefaultBackground(TCODColor::darkestGrey);
	con->clear();
	renderBar(1, 1, BAR_WIDTH, "HP", engine.player->destructible->hp,
		engine.player->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed);

	// draw the message log
	int y = 1;
	float colorCoef = 1.0f;
	for (int i = log.size() - 1; i >= 0; i--) {
		Message *message = log.get(i);
		con->setDefaultForeground(message->col * colorCoef);
		con->print(MSG_X, y, message->text);
		y++;
		if (y > MSG_HEIGHT) {
			break;
		}
		if (colorCoef > 0.4f) {
			colorCoef -= 0.3f;
		}
	}

	renderMouseLook();

	// dungeon level
	con->setDefaultForeground(TCODColor::white);
	con->print(1, 4, "Dungeon level %d", engine.level);

	// player xp bar
	PlayerAi *ai = (PlayerAi *)engine.player->ai;
	char xpTxt[128];
	sprintf_s(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 3, BAR_WIDTH, xpTxt, engine.player->destructible->xp,
		ai->getNextLevelXp(),
		TCODColor::lightViolet, TCODColor::darkerViolet);

	con->print(3, 2, "Atk: %0.0f, Def: %0.0f", engine.player->attacker->power, engine.player->destructible->defense);

	TCODConsole::blit(con, 0, 0, engine.screenWidth, PANEL_HEIGHT, TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);
}

void Gui::message(const TCODColor & col, const char * text, ...)
{
	va_list args;
	char buf[128];
	va_start(args, text);
	vsprintf_s(buf, 128, text, args);
	va_end(args);

	Message *msg = new Message(buf, col);
	puts(buf);
	log.push(msg);
}


void Gui::renderBar(int x, int y, int width, const char *name, float value, float maxValue, const TCODColor & barColor, const TCODColor & backColor)
{
	int barHeight = 1;
	// back of bar
	con->setDefaultBackground(backColor);
	con->rect(x, y, width, barHeight, false, TCOD_BKGND_SET);

	int filledBarWidth = (int)(value / maxValue * width);
	if (filledBarWidth > 0) {
		con->setDefaultBackground(barColor);
		con->rect(x, y, filledBarWidth, barHeight, false, TCOD_BKGND_SET);
	}

	// print text on top
	con->setDefaultForeground(TCODColor::white);
	con->printEx(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s: %g/%g", name, value, maxValue);
}

void Gui::renderMouseLook()
{
	int mapx = 0;
	int mapy = 0;
	Camera::getInstance().to_map(engine.mouse.cx, engine.mouse.cy, mapx, mapy);


	if (!engine.map->isInFov(mapx, mapy)) {
		return;
	}

	char buf[128] = ""; // empty string with \0

	bool first = true;
	for (Actor **it = engine.actors.begin(); it != engine.actors.end(); it++) {
		Actor *actor = *it;
		// under the mouse cursor
		if (actor->x == mapx && actor->y == mapy) {
			if (!first) {
				strcat_s(buf, 128, ", ");
			}
			else {
				first = false;
			}
			strcat_s(buf, 128, actor->name);
		}
	}

	con->setDefaultForeground(TCODColor::lightGrey);
	con->print(1, 0, buf);
}

Gui::Message::Message(const char * text, const TCODColor &col) :
	text(_strdup(text)), col(col)
{
}

Gui::Message::~Message()
{
	free(text);
}


void Gui::showHelp() {
	int menux, menuy;
	int width = 50; int height = 20;
	menux = engine.screenWidth / 2 - width / 2;
	menuy = engine.screenHeight / 2 - height / 2;
	TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
	TCODConsole::root->printFrame(menux, menuy, width, height, true,
		TCOD_BKGND_ALPHA(70), "Help");

	while (!TCODConsole::isWindowClosed()) {
		TCODConsole::root->print(menux + 2, menuy + 3, "Press arrows or numpad buttons to move. \nUse 'g' to pick up items, \n'i' to open an inventory, 'd' to drop item. \n'c' to open the character information screen,\n '<' or ',' to move down the stairs, \n'?' or '/' for this help.\nEsc to open the main menu. \n\nPress any key to close this window.");

		TCODConsole::flush();
		TCOD_key_t key;
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, true);
		if (key.vk != TCODK_NONE) {
			return;
		}
	}
}


Menu::~Menu()
{
	clear();
}

void Menu::clear()
{
	items.clearAndDelete();
}

void Menu::addItem(MenuItemCode code, const char * label)
{
	MenuItem *item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push(item);
}

Menu::MenuItemCode Menu::pick(DisplayMode mode)
{
	int selectedItem = 0;
	int menux, menuy;
	if (mode == DisplayMode::PAUSE) {
		menux = engine.screenWidth / 2 - PAUSE_MENU_WIDTH / 2;
		menuy = engine.screenHeight / 2 - PAUSE_MENU_HEIGHT / 2;
		TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
		TCODConsole::root->printFrame(menux, menuy, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, true,
			TCOD_BKGND_ALPHA(70), "menu");
		menux += 2;
		menuy += 3;
	}
	else {
		menux = 10;
		menuy = TCODConsole::root->getHeight() / 3;

		TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
		TCODConsole::root->print(menux, menuy, "Dungeons of Despair");
		TCODConsole::root->print(menux, menuy + 20, "by kuroneko1996");

		menuy += 6;
	}

	while (!TCODConsole::isWindowClosed()) {
		int currentItem = 0;

		for (MenuItem **it = items.begin(); it < items.end(); it++) {
			if (currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			}
			else {
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->print(menux, menuy + currentItem * 3, (*it)->label);
			currentItem++;
		}

		TCODConsole::flush();

		TCOD_key_t key;
		TCOD_mouse_t mouse;
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &key, &mouse, true);

		int mselected = selectWithMouse(mouse.cx, mouse.cy, menux, menuy);
		if (mselected != -1) {
			selectedItem = mselected;
			if (mouse.lbutton_pressed) {
				return(items.get(mselected)->code);
			}
		}

		switch (key.vk) {
		case TCODK_UP:
		case TCODK_KP8:
			selectedItem--;
			if (selectedItem < 0) {
				selectedItem = items.size() - 1;
			}
			break;
		case TCODK_DOWN:
		case TCODK_KP2:
			selectedItem = (selectedItem + 1) % items.size();
			break;
		case TCODK_ENTER:
		case TCODK_KP5:
		case TCODK_KPENTER:
			return items.get(selectedItem)->code;
			break;
		default:
			break;
		}
	}
	
	return MenuItemCode::NONE;
}

int Menu::selectWithMouse(int mousex, int mousey, int startx, int starty) {
	int width = 100;
	int height = 1;
	int y = starty;
	for (int i = 0; i < items.size(); i++) {
		if (!(mousex < startx || mousey < y || mousex >= startx+width || mousey >= y+height)) {
			return i;
		}
		y += 3;
	}
	return -1;
}
#pragma once

class Menu {
public:
	static const int PAUSE_MENU_WIDTH;
	static const int PAUSE_MENU_HEIGHT;

	enum MenuItemCode {
		NONE, NEW_GAME, CONTINUE, EXIT, CONSTITUTION, STRENGTH, AGILITY
	};
	enum DisplayMode {
		MAIN,
		PAUSE
	};

	~Menu();
	void clear();
	void addItem(MenuItemCode code, const char *label);
	MenuItemCode pick(DisplayMode mode=MAIN);
	int selectWithMouse(int mousex, int mousey, int startx, int starty);
protected:
	struct MenuItem {
		MenuItemCode code;
		const char* label;
	};
	TCODList<MenuItem *> items;
};


class Gui
{
public:
	static const int PANEL_HEIGHT;
	static const int BAR_WIDTH;
	static const int MSG_X;
	static const int MSG_HEIGHT;

	Menu menu;

	Gui();
	~Gui();
	void render();
	void message(const TCODColor &col, const char *text, ...);
	void showHelp();
	void clear();
protected:
	TCODConsole *con;
	struct Message {
		char *text;
		TCODColor col;
		Message(const char *text, const TCODColor &col);
		~Message();
	};
	TCODList<Message *> log;

	void renderBar(int x, int y, int width, const char *name, float value, float maxValue,
		const TCODColor &barColor, const TCODColor &backColor);
	void renderMouseLook();
};


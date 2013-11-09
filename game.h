#pragma once
#include "map.h"
#include "objects.h"
#include <map>

struct Game {
	enum { NORMAL_MODE, OPEN_MODE, CLOSE_MODE };

	Map map;
	Player player;
	std::vector<Door> doors;
	int mode;
	std::string message;

	Game();
	bool process(int ch);
private:
	static std::map<int, Point> directions;
	bool process_normal_mode(int ch);
	bool process_open_mode(int ch);
	bool process_close_mode(int ch);
};


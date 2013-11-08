#pragma once
#include "map.h"
#include "objects.h"

struct Game {
	enum { NORMAL_MODE, OPEN_MODE, CLOSE_MODE };

	Map map;
	Player player;
	std::vector<Door> doors;
	int mode;

	Game();
	bool process(int ch);
};


#pragma once
#include "util.h"

struct Player {
	Point pos;
	explicit Player(const Point & player_pos);
	Sprite sprite() const;
};

struct Door {
	Point pos;
	bool opened;
	explicit Door(const Point & door_pos = Point(), bool is_opened = false);
	Sprite sprite() const;
    operator bool() const;
};


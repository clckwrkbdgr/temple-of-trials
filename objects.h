#pragma once
#include "util.h"

struct Monster {
	Point pos;
	Sprite sprite;
	Monster(const Point & monster_pos, const Sprite & monster_sprite);
	Monster();
    operator bool() const;

	static Monster player(const Point & monster_pos = Point());
	static Monster ant(const Point & monster_pos = Point());
};

struct Door {
	Point pos;
	bool opened;
	explicit Door(const Point & door_pos = Point(), bool is_opened = false);
	Sprite sprite() const;
    operator bool() const;
};


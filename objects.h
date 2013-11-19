#pragma once
#include "util.h"

struct Monster {
	Point pos;
	Sprite sprite;
	int sight;
	int ai;
	std::string name;
	Monster(const Point & monster_pos, const Sprite & monster_sprite, int monster_sight, int monster_ai);
	Monster();
	operator bool() const;

	static Monster player(const Point & monster_pos);
	static Monster ant(int monster_ai, const Point & monster_pos);
};

struct Door {
	Point pos;
	Sprite opened_sprite, closed_sprite;
	bool opened;
	Door();
	Door(const Point & door_pos, const Sprite & opened_door_sprite, const Sprite & closed_door_sprite, bool is_opened);
	Sprite sprite() const;
	operator bool() const;
};


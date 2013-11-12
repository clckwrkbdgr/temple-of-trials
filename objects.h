#pragma once
#include "util.h"

struct Monster {
	enum AI { AI_PLAYER, AI_STILL, AI_WANDER };
	Point pos;
	Sprite sprite;
	int ai;
	std::string name;
	Monster(const Point & monster_pos, const Sprite & monster_sprite, int monster_ai);
	Monster();
	operator bool() const;

	static Monster player(const Point & monster_pos);
	static Monster ant(int monster_ai, const Point & monster_pos);
};

struct Door {
	Point pos;
	bool opened;
	explicit Door(const Point & door_pos = Point(), bool is_opened = false);
	Sprite sprite() const;
	operator bool() const;
};


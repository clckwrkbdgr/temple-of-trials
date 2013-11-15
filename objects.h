#pragma once
#include "util.h"

struct AI {
	enum Faction { DUMMY, PLAYER, MONSTER };
	enum Movement { STILL, WANDER };

	int faction;
	int movement;
	AI(int ai_faction = DUMMY, int ai_movement = STILL);
};

struct Monster {
	Point pos;
	Sprite sprite;
	AI ai;
	std::string name;
	Monster(const Point & monster_pos, const Sprite & monster_sprite, const AI & monster_ai);
	Monster();
	operator bool() const;

	static Monster player(const Point & monster_pos);
	static Monster ant(const AI & monster_ai, const Point & monster_pos);
};

struct Door {
	Point pos;
	bool opened;
	explicit Door(const Point & door_pos = Point(), bool is_opened = false);
	Sprite sprite() const;
	operator bool() const;
};


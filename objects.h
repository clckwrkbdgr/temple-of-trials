#pragma once
#include "util.h"

struct Monster {
	Point pos;
	Sprite sprite;
	int sight;
	int ai;
	int hp;
	std::string name;
	Monster();
	operator bool() const;
	bool is_dead() const { return hp <= 0; }

	struct Builder;
};
struct Monster::Builder {
	Monster monster;
	Builder & pos(const Point & value);
	Builder & sprite(const Sprite & sprite);
	Builder & sight(int value);
	Builder & ai(int value);
	Builder & hp(int value);
	Builder & name(const std::string & value);
};


struct Door {
	Point pos;
	Sprite opened_sprite, closed_sprite;
	bool opened;
	Door();
	Sprite sprite() const;
	operator bool() const;

	struct Builder;
};
struct Door::Builder {
	Door door;
	Builder & pos(const Point & value);
	Builder & opened_sprite(const Sprite & value);
	Builder & closed_sprite(const Sprite & value);
	Builder & opened(bool value);
};


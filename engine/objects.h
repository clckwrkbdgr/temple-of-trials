#pragma once
#include "util.h"
#include "items.h"

struct Object {
	Point pos;
	int sprite, opened_sprite;
	std::string name;
	std::vector<Item> items;
	bool passable, transparent;
	bool containable;
	bool drinkable;
	bool transporting;
	bool triggerable;
	bool openable, opened;
	int up_destination, down_destination;
	bool locked;
	int lock_type;
	Object();
	bool valid() const;
	bool is_passable() const;
	bool is_transparent() const;
	int get_sprite() const;
	bool is_exit_up() const;
	bool is_exit_down() const;

	struct Builder;
};
struct Object::Builder {
	Object result;
	operator Object() { return result; }
	Builder & pos(const Point & value);
	Builder & sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & item(const Item & value);
	Builder & passable();
	Builder & transparent();
	Builder & containable();
	Builder & drinkable();
	Builder & transporting();
	Builder & triggerable();
	Builder & up_destination(int value);
	Builder & down_destination(int value);
	Builder & openable();
	Builder & opened_sprite(const int & value);
	Builder & closed_sprite(const int & value);
	Builder & opened(bool value);
	Builder & locked(bool value);
	Builder & lock_type(int value);
};



#pragma once
#include "util.h"
#include "items.h"

struct ObjectType {
	std::string id;
	int sprite, opened_sprite;
	std::string name;
	bool passable, transparent;
	bool containable;
	bool drinkable;
	bool transporting;
	bool triggerable;
	bool openable;
	ObjectType(const std::string & type_id = std::string());

	struct Builder;
};
struct ObjectType::Builder {
	ObjectType result;
	Builder(const std::string & type_id) : result(type_id) {}
	operator ObjectType() { return result; }
	Builder & sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & passable();
	Builder & transparent();
	Builder & containable();
	Builder & drinkable();
	Builder & transporting();
	Builder & triggerable();
	Builder & openable();
	Builder & opened_sprite(const int & value);
	Builder & closed_sprite(const int & value);
};

struct Object {
	typedef ObjectType Type;
	TypePtr<Type> type;

	Point pos;
	std::vector<Item> items;
	bool opened;
	int up_destination, down_destination;
	bool locked;
	int lock_type;
	Object(const Type * object_type = 0);
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
	Builder(const Type * type) : result(type) {}
	operator Object() { return result; }
	Builder & pos(const Point & value);
	Builder & item(const Item & value);
	Builder & up_destination(int value);
	Builder & down_destination(int value);
	Builder & opened(bool value);
	Builder & locked(bool value);
	Builder & lock_type(int value);
};



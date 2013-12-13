#pragma once
#include "util.h"
#include <list>
class Control;
class Item;

struct Monster {
	enum Faction { NEUTRAL, PLAYER, MONSTER };

	int faction;
	bool godmode;
	Point pos;
	int sprite;
	int sight;
	int ai;
	int max_hp, hp;
	int hit_strength;
	std::string name;
	std::vector<Item> inventory;
	int wielded;
	int worn;
	bool poisonous;
	int poisoning;
	std::list<Control> plan;
	Monster();
	operator bool() const;
	bool is_dead() const { return hp <= 0; }
	int damage() const;
	Item & wielded_item();
	const Item & wielded_item() const;
	Item & worn_item();
	const Item & worn_item() const;

	struct Builder;
};
struct Monster::Builder {
	Monster result;
	operator Monster() { return result; }
	Builder & faction(int value);
	Builder & pos(const Point & value);
	Builder & sprite(const int & sprite);
	Builder & sight(int value);
	Builder & ai(int value);
	Builder & hp(int value);
	Builder & name(const std::string & value);
	Builder & item(const Item & value);
	Builder & hit_strength(int value);
	Builder & poisonous(bool value);
	Builder & wield(int value);
	Builder & wear(int value);
};


struct Door {
	Point pos;
	int opened_sprite, closed_sprite;
	std::string name;
	bool opened;
	Door();
	int get_sprite() const;
	operator bool() const;
	bool is_passable() const { return opened; }
	bool is_transparent() const { return opened; }

	struct Builder;
};
struct Door::Builder {
	Door result;
	operator Door() { return result; }
	Builder & pos(const Point & value);
	Builder & opened_sprite(const int & value);
	Builder & closed_sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & opened(bool value);
};


struct Item {
	Point pos;
	int sprite;
	std::string name;
	int damage;
	bool wearable;
	int defence;
	bool edible;
	int antidote;
	int healing;
	bool quest;
	Item();
	operator bool() const;

	struct Builder;
};
struct Item::Builder {
	Item result;
	operator Item() { return result; }
	Builder & pos(const Point & value);
	Builder & sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & damage(int damage);
	Builder & wearable();
	Builder & defence(int defence);
	Builder & edible();
	Builder & antidote(int value);
	Builder & healing(int value);
	Builder & quest();
};


struct Object {
	Point pos;
	int sprite;
	std::string name;
	std::vector<Item> items;
	bool passable;
	bool containable;
	bool drinkable;
	bool transporting;
	int up_destination, down_destination;
	Object();
	operator bool() const;
	bool is_passable() const { return passable; }
	bool is_transparent() const { return true; }
	int get_sprite() const { return sprite; }

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
	Builder & containable();
	Builder & drinkable();
	Builder & transporting();
	Builder & up_destination(int value);
	Builder & down_destination(int value);
};

struct Trap {
	Point pos;
	int sprite;
	std::string name;
	bool triggered;
	Item bolt;
	Trap();
	operator bool() const;
	bool is_passable() const { return true; }
	bool is_transparent() const { return true; }
	int get_sprite() const { return sprite; }

	struct Builder;
};
struct Trap::Builder {
	Trap result;
	operator Trap() { return result; }
	Builder & pos(const Point & value);
	Builder & sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & bolt(const Item & value);
};


#pragma once
#include "util.h"
#include <list>
class Item;
class Action;

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
	std::list<Action*> plan;
	Monster();
	~Monster();
	operator bool() const;
	bool is_dead() const { return hp <= 0; }
	int damage() const;
	Item & wielded_item();
	const Item & wielded_item() const;
	Item & worn_item();
	const Item & worn_item() const;
	const Item & quest_item() const;
	bool is_valid_slot(unsigned slot) const;
	bool has_key(int key_type) const;
	void add_path(const std::list<Point> & path);

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
	int key_type;
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
	Builder & key_type(int value);
};

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
	operator bool() const;
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



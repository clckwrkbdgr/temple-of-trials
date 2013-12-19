#pragma once
#include "util.h"
#include <list>
class Item;
class Action;

struct Inventory {
	enum { SLOT_COUNT = 26 };
	static unsigned NOTHING;

	unsigned wielded, worn;
	std::vector<Item> items;

	Inventory();
	bool empty() const;
	void clear();
	unsigned size() const;
	bool set_item(unsigned slot, const Item & item);
	const Item & get_item(unsigned slot) const;
	Item & get_item(unsigned slot);

	unsigned insert(const Item & item);
	Item take_item(unsigned slot);
	Item take_first_item();
	Item take_wielded_item();
	Item take_worn_item();

	bool wield(unsigned slot);
	bool wields(unsigned slot) const;
	void unwield();
	const Item & wielded_item() const { return get_item(wielded); }
	Item & wielded_item() { return get_item(wielded); }

	bool wear(unsigned slot);
	bool wears(unsigned slot) const;
	void take_off();
	const Item & worn_item() const { return get_item(worn); }
	Item & worn_item() { return get_item(worn); }

	const Item & quest_item() const;
	bool has_key(int key_type) const;
};

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
	Inventory inventory;
	bool poisonous;
	int poisoning;
	std::list<Action*> plan;
	Monster();
	~Monster();
	bool valid() const;
	bool is_dead() const { return hp <= 0; }
	int damage() const;
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
	bool valid() const;

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



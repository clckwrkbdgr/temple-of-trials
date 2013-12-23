#pragma once
#include "map.h"
#include <string>

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


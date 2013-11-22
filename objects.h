#pragma once
#include "util.h"
class Item;

struct Monster {
	Point pos;
	Sprite sprite;
	int sight;
	int ai;
	int hp;
	int hit_strength;
	std::string name;
	std::vector<Item> inventory;
	int wielded;
	Monster();
	operator bool() const;
	bool is_dead() const { return hp <= 0; }
	int damage() const;

	struct Builder;
};
struct Monster::Builder {
	Monster result;
	operator Monster() { return result; }
	Builder & pos(const Point & value);
	Builder & sprite(const Sprite & sprite);
	Builder & sight(int value);
	Builder & ai(int value);
	Builder & hp(int value);
	Builder & name(const std::string & value);
	Builder & item(const Item & value);
	Builder & hit_strength(int value);
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
	Door result;
	operator Door() { return result; }
	Builder & pos(const Point & value);
	Builder & opened_sprite(const Sprite & value);
	Builder & closed_sprite(const Sprite & value);
	Builder & opened(bool value);
};


struct Item {
	Point pos;
	Sprite sprite;
	std::string name;
	int damage;
	Item();
	operator bool() const;

	struct Builder;
};
struct Item::Builder {
	Item result;
	operator Item() { return result; }
	Builder & pos(const Point & value);
	Builder & sprite(const Sprite & value);
	Builder & name(const std::string & value);
	Builder & damage(int damage);
};


struct Container {
	Point pos;
	Sprite sprite;
	std::string name;
	std::vector<Item> items;
	Container();
	operator bool() const;

	struct Builder;
};
struct Container::Builder {
	Container result;
	operator Container() { return result; }
	Builder & pos(const Point & value);
	Builder & sprite(const Sprite & value);
	Builder & name(const std::string & value);
	Builder & item(const Item & value);
};


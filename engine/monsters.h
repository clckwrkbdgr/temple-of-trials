#pragma once
#include "items.h"
class Action;

struct Monster {
	enum Faction { NEUTRAL, PLAYER, MONSTER };

	int faction;
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


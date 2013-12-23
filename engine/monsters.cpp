#include "monsters.h"
#include "actions.h"
#include "util.h"

Monster::Monster()
	: faction(NEUTRAL), sprite(0), sight(0), ai(0), max_hp(1), hp(max_hp),
	hit_strength(0), poisonous(false), poisoning(0)
{
}

bool Monster::valid() const
{
	return pos.valid();
}

Monster::~Monster()
{
	foreach(Action * action, plan) {
		delete action;
	}
}

int Monster::damage() const
{
	if(inventory.wielded_item().valid()) {
		return inventory.wielded_item().damage;
	}
	return hit_strength;
}

void Monster::add_path(const std::list<Point> & path)
{
	foreach(const Point & shift, path) {
		plan.push_back(new Move(shift));
	}
}

Monster::Builder & Monster::Builder::faction(int value) { result.faction = value; return *this; }
Monster::Builder & Monster::Builder::pos(const Point & value) { result.pos = value; return *this; }
Monster::Builder & Monster::Builder::sprite(const int & value) { result.sprite = value; return *this; }
Monster::Builder & Monster::Builder::sight(int value) { result.sight = value; return *this; }
Monster::Builder & Monster::Builder::ai(int value) { result.ai = value; return *this; }
Monster::Builder & Monster::Builder::hp(int value) { result.hp = result.max_hp = value; return *this; }
Monster::Builder & Monster::Builder::name(const std::string & value) { result.name = value; return *this; }
Monster::Builder & Monster::Builder::item(const Item & value) { result.inventory.insert(value); return *this; }
Monster::Builder & Monster::Builder::hit_strength(int value) { result.hit_strength = value; return *this; }
Monster::Builder & Monster::Builder::poisonous(bool value) { result.poisonous = value; return *this; }
Monster::Builder & Monster::Builder::wield(int value) { result.inventory.wield(value); return *this; }
Monster::Builder & Monster::Builder::wear(int value) { result.inventory.wear(value); return *this; }



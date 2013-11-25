#include "objects.h"

Monster::Monster()
	: sprite(' '), sight(0), ai(0), max_hp(1), hp(max_hp), hit_strength(0), wielded(-1), worn(-1), poisonous(false)
{
}

Monster::operator bool() const
{
    return bool(pos);
}

int Monster::damage() const
{
	if(wielded_item()) {
		return wielded_item().damage;
	}
	return hit_strength;
}

Item & Monster::wielded_item()
{
	static Item empty;
	empty = Item();
	if(wielded < 0) {
		return empty;
	}
	if(wielded >= int(inventory.size())) {
		log("{0} was wielding incorrect slot: {1}", name, wielded);
		wielded = -1;
		return empty;
	}
	Item & item = inventory[wielded];
	if(!item) {
		log("{0} was wielding empty slot: {1}", name, wielded);
		wielded = -1;
		return empty;
	}
	return item;
}

const Item & Monster::wielded_item() const
{
	static Item empty;
	if(wielded < 0) {
		return empty;
	}
	if(wielded >= int(inventory.size())) {
		log("{0} was wielding incorrect slot: {1}", name, wielded);
		return empty;
	}
	const Item & item = inventory[wielded];
	if(!item) {
		log("{0} was wielding empty slot: {1}", name, wielded);
		return empty;
	}
	return item;
}

Item & Monster::worn_item()
{
	static Item empty;
	empty = Item();
	if(worn < 0) {
		return empty;
	}
	if(worn >= int(inventory.size())) {
		log("{0} was wearing incorrect slot: {1}", name, worn);
		worn = -1;
		return empty;
	}
	Item & item = inventory[worn];
	if(!item) {
		log("{0} was wearing empty slot: {1}", name, worn);
		worn = -1;
		return empty;
	}
	return item;
}

const Item & Monster::worn_item() const
{
	static Item empty;
	if(worn < 0) {
		return empty;
	}
	if(worn >= int(inventory.size())) {
		log("{0} was wearing incorrect slot: {1}", name, worn);
		return empty;
	}
	const Item & item = inventory[worn];
	if(!item) {
		log("{0} was wearing empty slot: {1}", name, worn);
		return empty;
	}
	return item;
}

Monster::Builder & Monster::Builder::pos(const Point & value) { result.pos = value; return *this; }
Monster::Builder & Monster::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Monster::Builder & Monster::Builder::sight(int value) { result.sight = value; return *this; }
Monster::Builder & Monster::Builder::ai(int value) { result.ai = value; return *this; }
Monster::Builder & Monster::Builder::hp(int value) { result.hp = result.max_hp = value; return *this; }
Monster::Builder & Monster::Builder::name(const std::string & value) { result.name = value; return *this; }
Monster::Builder & Monster::Builder::item(const Item & value) { result.inventory.push_back(value); return *this; }
Monster::Builder & Monster::Builder::hit_strength(int value) { result.hit_strength = value; return *this; }
Monster::Builder & Monster::Builder::poisonous(bool value) { result.poisonous = value; return *this; }


Door::Door()
	: opened_sprite(' '), closed_sprite(' '), opened(false)
{
}

Sprite Door::sprite() const
{
	return opened ? opened_sprite : closed_sprite;
}

Door::operator bool() const
{
    return bool(pos);
}

Door::Builder & Door::Builder::pos(const Point & value) { result.pos = value; return *this; }
Door::Builder & Door::Builder::opened_sprite(const Sprite & value) { result.opened_sprite = value; return *this; }
Door::Builder & Door::Builder::closed_sprite(const Sprite & value) { result.closed_sprite = value; return *this; }
Door::Builder & Door::Builder::opened(bool value) { result.opened = value; return *this; }


Item::Item()
	: sprite(' '), damage(0), wearable(false), defence(0)
{
}

Item::operator bool() const
{
	return bool(pos);
}

Item::Builder & Item::Builder::pos(const Point & value) { result.pos = value; return *this; }
Item::Builder & Item::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Item::Builder & Item::Builder::name(const std::string & value) { result.name = value; return *this; }
Item::Builder & Item::Builder::damage(int value) { result.damage = value; return *this; }
Item::Builder & Item::Builder::wearable() { result.wearable = true; return *this; }
Item::Builder & Item::Builder::defence(int value) { result.defence = value; return *this; }


Container::Container()
	: sprite(' ')
{
}

Container::operator bool() const
{
	return bool(pos);
}

Container::Builder & Container::Builder::pos(const Point & value) { result.pos = value; return *this; }
Container::Builder & Container::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Container::Builder & Container::Builder::name(const std::string & value) { result.name = value; return *this; }
Container::Builder & Container::Builder::item(const Item & value) { result.items.push_back(value); return *this; }


Fountain::Fountain()
	: sprite(' ')
{
}

Fountain::operator bool() const
{
	return bool(pos);
}

Fountain::Builder & Fountain::Builder::pos(const Point & value) { result.pos = value; return *this; }
Fountain::Builder & Fountain::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Fountain::Builder & Fountain::Builder::name(const std::string & value) { result.name = value; return *this; }

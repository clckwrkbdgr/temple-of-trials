#include "objects.h"
#include "game.h"

Monster::Monster()
	: faction(NEUTRAL), godmode(false), sprite(0), sight(0), ai(0), max_hp(1), hp(max_hp),
	hit_strength(0), wielded(-1), worn(-1), poisonous(false), poisoning(0)
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

Monster::Builder & Monster::Builder::faction(int value) { result.faction = value; return *this; }
Monster::Builder & Monster::Builder::pos(const Point & value) { result.pos = value; return *this; }
Monster::Builder & Monster::Builder::sprite(const int & value) { result.sprite = value; return *this; }
Monster::Builder & Monster::Builder::sight(int value) { result.sight = value; return *this; }
Monster::Builder & Monster::Builder::ai(int value) { result.ai = value; return *this; }
Monster::Builder & Monster::Builder::hp(int value) { result.hp = result.max_hp = value; return *this; }
Monster::Builder & Monster::Builder::name(const std::string & value) { result.name = value; return *this; }
Monster::Builder & Monster::Builder::item(const Item & value) { result.inventory.push_back(value); return *this; }
Monster::Builder & Monster::Builder::hit_strength(int value) { result.hit_strength = value; return *this; }
Monster::Builder & Monster::Builder::poisonous(bool value) { result.poisonous = value; return *this; }
Monster::Builder & Monster::Builder::wield(int value) { result.wielded = value; return *this; }
Monster::Builder & Monster::Builder::wear(int value) { result.worn = value; return *this; }


Item::Item()
	: sprite(0), damage(0), wearable(false), defence(0), edible(false), antidote(0), healing(0), quest(false)
{
}

Item::operator bool() const
{
	return bool(sprite);
}

Item::Builder & Item::Builder::pos(const Point & value) { result.pos = value; return *this; }
Item::Builder & Item::Builder::sprite(const int & value) { result.sprite = value; return *this; }
Item::Builder & Item::Builder::name(const std::string & value) { result.name = value; return *this; }
Item::Builder & Item::Builder::damage(int value) { result.damage = value; return *this; }
Item::Builder & Item::Builder::wearable() { result.wearable = true; return *this; }
Item::Builder & Item::Builder::defence(int value) { result.defence = value; return *this; }
Item::Builder & Item::Builder::edible() { result.edible = true; return *this; }
Item::Builder & Item::Builder::antidote(int value) { result.antidote = value; return *this; }
Item::Builder & Item::Builder::healing(int value) { result.healing = value; return *this; }
Item::Builder & Item::Builder::quest() { result.quest = true; return *this; }


Object::Object()
	: sprite(0), opened_sprite(0), passable(false), transparent(false), containable(false), drinkable(false), transporting(false), triggerable(false), openable(false), opened(false)
{
}

Object::operator bool() const
{
	return bool(pos);
}

Object::Builder & Object::Builder::pos(const Point & value) { result.pos = value; return *this; }
Object::Builder & Object::Builder::sprite(const int & value) { result.sprite = value; result.opened_sprite = value; return *this; }
Object::Builder & Object::Builder::name(const std::string & value) { result.name = value; return *this; }
Object::Builder & Object::Builder::item(const Item & value) { result.items.push_back(value); return *this; }
Object::Builder & Object::Builder::passable() { result.passable = true; return *this; }
Object::Builder & Object::Builder::transparent() { result.transparent = true; return *this; }
Object::Builder & Object::Builder::containable() { result.containable = true; return *this; }
Object::Builder & Object::Builder::drinkable() { result.drinkable = true; return *this; }
Object::Builder & Object::Builder::transporting() { result.transporting = true; return *this; }
Object::Builder & Object::Builder::triggerable() { result.triggerable = true; return *this; }
Object::Builder & Object::Builder::up_destination(int value) { result.up_destination = value; return *this; }
Object::Builder & Object::Builder::down_destination(int value) { result.down_destination = value; return *this; }
Object::Builder & Object::Builder::opened_sprite(const int & value) { result.opened_sprite = value; return *this; }
Object::Builder & Object::Builder::closed_sprite(const int & value) { result.sprite = value; return *this; }
Object::Builder & Object::Builder::openable() { result.openable = true; return *this; }
Object::Builder & Object::Builder::opened(bool value) { result.opened = value; return *this; }

int Object::get_sprite() const
{
	if(openable) {
		return opened ? opened_sprite : sprite;
	}
	return sprite;
}

bool Object::is_passable() const
{
	if(openable) {
		return opened ? passable : false;
	}
	return passable;
}

bool Object::is_transparent() const
{
	if(openable) {
		return opened ? transparent : false;
	}
	return transparent;
}


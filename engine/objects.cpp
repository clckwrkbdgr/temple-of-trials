#include "objects.h"
#include "game.h"
#include "actions.h"

unsigned Inventory::NOTHING = static_cast<unsigned>(-1);

Inventory::Inventory()
	: wielded(NOTHING), worn(NOTHING)
{
}

bool Inventory::empty() const
{
	foreach(const Item & item, items) {
		if(item) {
			return false;
		}
	}
	return true;
}

bool Inventory::set_item(unsigned slot, const Item & item)
{
	if(slot < 0 || SLOT_COUNT <= slot) {
		return false;
	}
	if(slot >= items.size()) {
		items.resize(slot + 1);
	}
	items[slot] = item;
	return true;
}

Item Inventory::take_item(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		return Item();
	}
	if(wielded == slot) {
		unwield();
	}
	if(worn == slot) {
		take_off();
	}
	Item result = items[slot];
	items[slot] = Item();
	return result;
}

Item Inventory::take_first_item()
{
	for(unsigned slot = 0; slot < items.size(); ++slot) {
		if(items[slot]) {
			return take_item(slot);
		}
	}
	return Item();
}

Item Inventory::take_wielded_item()
{
	return take_item(wielded);
}

Item Inventory::take_worn_item()
{
	return take_item(worn);
}

bool Inventory::wield(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		return false;
	}
	wielded = slot;
	return true;
}

bool Inventory::wields(unsigned slot) const
{
	return wielded == slot;
}

void Inventory::unwield()
{
	wielded = NOTHING;
}

bool Inventory::wears(unsigned slot) const
{
	return worn == slot;
}

bool Inventory::wear(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		return false;
	}
	worn = slot;
	return true;
}

void Inventory::take_off()
{
	worn = NOTHING;
}

const Item & Inventory::get_item(unsigned slot) const
{
	if(slot < 0 || items.size() <= slot) {
		static Item empty;
		return empty;
	}
	return items[slot];
}

Item & Inventory::get_item(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		static Item empty;
		empty = Item();
		return empty;
	}
	return items[slot];
}

void Inventory::clear()
{
	wielded = NOTHING;
	worn = NOTHING;
	items.clear();
}

unsigned Inventory::insert(const Item & item)
{
	if(items.size() >= SLOT_COUNT) {
		return NOTHING;
	}
	for(unsigned slot = 0; slot < items.size(); ++slot) {
		if(!items[slot]) {
			items[slot] = item;
			return slot;
		}
	}
	if(items.size() < SLOT_COUNT) {
		items.resize(items.size() + 1);
	}
	items[items.size() - 1] = item;
	return items.size() - 1;
}

const Item & Inventory::quest_item() const
{
	foreach(const Item & item, items) {
		if(item.quest) {
			return item;
		}
	}
	static Item empty;
	return empty;
}

bool Inventory::has_key(int key_type) const
{
	foreach(const Item & item, items) {
		if(item.key_type == key_type) {
			return true;
		}
	}
	return false;
}


Monster::Monster()
	: faction(NEUTRAL), godmode(false), sprite(0), sight(0), ai(0), max_hp(1), hp(max_hp),
	hit_strength(0), poisonous(false), poisoning(0)
{
}

Monster::~Monster()
{
	foreach(Action * action, plan) {
		delete action;
	}
}

Monster::operator bool() const
{
    return bool(pos);
}

int Monster::damage() const
{
	if(inventory.wielded_item()) {
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
Item::Builder & Item::Builder::key_type(int value) { result.key_type = value; return *this; }


Object::Object()
	: sprite(0), opened_sprite(0), passable(false), transparent(false), containable(false), drinkable(false), transporting(false), triggerable(false), openable(false), opened(false)
{
}

Object::operator bool() const
{
	return bool(pos);
}

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

bool Object::is_exit_up() const
{
	return up_destination < 0;
}

bool Object::is_exit_down() const
{
	return down_destination < 0;
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
Object::Builder & Object::Builder::locked(bool value) { result.locked = value; return *this; }
Object::Builder & Object::Builder::lock_type(int value) { result.lock_type = value; return *this; }


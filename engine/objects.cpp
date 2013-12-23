#include "objects.h"
#include "game.h"
#include "items.h"
#include "actions.h"

Object::Object()
	: sprite(0), opened_sprite(0), passable(false), transparent(false), containable(false), drinkable(false), transporting(false), triggerable(false), openable(false), opened(false)
{
}

bool Object::valid() const
{
	return pos.valid();
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


#include "objects.h"
#include "game.h"
#include "items.h"
#include "actions.h"

ObjectType::ObjectType(const std::string & type_id)
	: id(type_id), sprite(0), opened_sprite(0), passable(false), transparent(false),
	containable(false), drinkable(false), transporting(false), triggerable(false), openable(false)
{
}

ObjectType::Builder & ObjectType::Builder::sprite(const int & value) { result.sprite = value; result.opened_sprite = value; return *this; }
ObjectType::Builder & ObjectType::Builder::name(const std::string & value) { result.name = value; return *this; }
ObjectType::Builder & ObjectType::Builder::passable() { result.passable = true; return *this; }
ObjectType::Builder & ObjectType::Builder::transparent() { result.transparent = true; return *this; }
ObjectType::Builder & ObjectType::Builder::containable() { result.containable = true; return *this; }
ObjectType::Builder & ObjectType::Builder::drinkable() { result.drinkable = true; return *this; }
ObjectType::Builder & ObjectType::Builder::transporting() { result.transporting = true; return *this; }
ObjectType::Builder & ObjectType::Builder::triggerable() { result.triggerable = true; return *this; }
ObjectType::Builder & ObjectType::Builder::opened_sprite(const int & value) { result.opened_sprite = value; return *this; }
ObjectType::Builder & ObjectType::Builder::closed_sprite(const int & value) { result.sprite = value; return *this; }
ObjectType::Builder & ObjectType::Builder::openable() { result.openable = true; return *this; }


Object::Object(const Type * object_type)
	: type(object_type), opened(false), up_destination(0), down_destination(0), locked(false), lock_type(0)
{
}

bool Object::valid() const
{
	return pos.valid();
}

int Object::get_sprite() const
{
	if(type->openable) {
		return opened ? type->opened_sprite : type->sprite;
	}
	return type->sprite;
}

bool Object::is_passable() const
{
	if(type->openable) {
		return opened ? type->passable : false;
	}
	return type->passable;
}

bool Object::is_transparent() const
{
	if(type->openable) {
		return opened ? type->transparent : false;
	}
	return type->transparent;
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
Object::Builder & Object::Builder::item(const Item & value) { result.items.push_back(value); return *this; }
Object::Builder & Object::Builder::up_destination(int value) { result.up_destination = value; return *this; }
Object::Builder & Object::Builder::down_destination(int value) { result.down_destination = value; return *this; }
Object::Builder & Object::Builder::opened(bool value) { result.opened = value; return *this; }
Object::Builder & Object::Builder::locked(bool value) { result.locked = value; return *this; }
Object::Builder & Object::Builder::lock_type(int value) { result.lock_type = value; return *this; }


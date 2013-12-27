#include "actions.h"
#include "monsters.h"
#include "game.h"

void Move::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster.valid(), game.messages.bumps_into(someone, monster));
    Object & object = find_at(game.level.objects, new_pos);
	ACTION_ASSERT(!object.valid() || object.type->passable, game.messages.bumps_into(someone, object));
	ACTION_ASSERT(game.cell_type_at(new_pos).passable, game.messages.bumps_into(someone, game.cell_type_at(new_pos)));
    someone.pos = new_pos;
}

void Drink::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster.valid(), game.messages.cannot_drink(someone, monster));
    Object & object = find_at(game.level.objects, new_pos);
	ACTION_ASSERT(object.valid() && object.type->drinkable, game.messages.nothing_to_drink(object));
	game.messages.drinks(someone, object);
	someone.heal_by(1);
}

void Open::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
	ACTION_ASSERT(object.valid() && (object.type->openable || object.type->containable), game.messages.nothing_to_open());
    if(object.type->openable) {
		ACTION_ASSERT(!object.opened(), game.messages.already_opened(object));
		if(object.locked) {
			ACTION_ASSERT(someone.inventory.has_key(object.lock_type), game.messages.is_locked(object));
			game.messages.unlocks(someone, object);
			object.locked = false;
		}
		object.open();
		game.messages.opened(someone, object);
    } else if(object.type->containable) {
		ACTION_ASSERT(!object.items.empty(), game.messages.is_empty(object));
		foreach(Item & item, object.items) {
			item.pos = someone.pos;
			game.level.items.push_back(item);
			game.messages.tooks_up_from(someone, item, object);
		}
		object.items.clear();
	}
}

void Close::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
    ACTION_ASSERT(object.valid() && object.type->openable, game.messages.nothing_to_close());
    ACTION_ASSERT(object.opened(), game.messages.already_closed(object));
    object.close();
    game.messages.closed(someone, object);
}

void Swing::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	if(monster.valid()) {
		game.hit(someone, monster, someone.damage());
		return;
	}
    Object & object = find_at(game.level.objects, new_pos);
	if(object.valid()) {
		if(object.type->openable && !object.opened()) {
			game.messages.swings_at_object(someone, object);
			if(object.locked) {
				ACTION_ASSERT(someone.inventory.has_key(object.lock_type), game.messages.is_locked(object));
				game.messages.unlocks(someone, object);
				object.locked = false;
			}
			object.open();
			game.messages.opened(someone, object);
		} else {
			game.messages.swings_at_object(someone, object);
		}
		return;
	}
	if(!game.cell_type_at(new_pos).passable) {
		game.messages.hits(someone, game.cell_type_at(new_pos));
		return;
	}
    game.messages.swings_at_nothing(someone);
}

void Fire::commit(Monster & someone, Game & game)
{
	Item item = someone.inventory.take_wielded_item();
	ACTION_ASSERT(item.valid(), game.messages.nothing_to_throw(someone));
    item.pos = someone.pos;
	game.messages.throws(someone, item);
	while(true) {
		if(!game.level.map.valid(item.pos)) {
			break;
		}
		if(!game.cell_type_at(item.pos + shift).transparent) {
			game.messages.hits(item, game.cell_type_at(item.pos + shift));
			game.level.items.push_back(item);
			break;
		}
		Object & object = find_at(game.level.objects, item.pos + shift);
		if(object.valid()) {
			if(object.type->containable) {
				game.messages.falls(item, object);
				object.items.push_back(item);
				break;
			} else if(object.type->drinkable) {
				game.messages.falls_and_lost(item, object);
				break;
			} else if(!object.type->transparent) {
				game.messages.hits(item, object);
				game.level.items.push_back(item);
				break;
			}
		}
		Monster & monster = find_at(game.level.monsters, item.pos + shift);
		if(monster.valid()) {
			game.messages.hits(item, monster);
			item.pos += shift;
			game.level.items.push_back(item);
			game.hit(someone, monster, item.type->damage);
			break;
		}
		item.pos += shift;
	}
}


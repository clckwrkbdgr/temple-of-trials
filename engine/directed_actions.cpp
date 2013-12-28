#include "actions.h"
#include "monsters.h"
#include "game.h"

void Move::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	if(monster.valid()) {
		game.messages.bumps_into(someone, monster);
		return;
	}
    Object & object = find_at(game.level.objects, new_pos);
	if(object.valid() && !object.type->passable) {
		game.messages.bumps_into(someone, object);
		return;
	}
	if(game.cell_type_at(new_pos).passable) {
		game.messages.bumps_into(someone, game.cell_type_at(new_pos));
		return;
	}
    someone.pos = new_pos;
}

void Drink::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	assert(!monster.valid(), CANNOT_DRINK, someone, monster);
    Object & object = find_at(game.level.objects, new_pos);
	assert(object.valid() && object.type->drinkable, NOTHING_TO_DRINK, someone, object);
	game.messages.drinks(someone, object);
	someone.heal_by(1);
}

void Open::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
	assert(object.valid() && (object.type->openable || object.type->containable), NOTHING_TO_OPEN, someone);
    if(object.type->openable) {
		assert(!object.opened(), ALREADY_OPENED, object);
		if(object.locked) {
			assert(someone.inventory.has_key(object.lock_type), LOCKED, object);
			game.messages.unlocks(someone, object);
			object.locked = false;
		}
		object.open();
		game.messages.opened(someone, object);
    } else if(object.type->containable) {
		assert(!object.items.empty(), HAS_NO_ITEMS, object);
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
    assert(object.valid() && object.type->openable, NOTHING_TO_CLOSE, someone);
    assert(object.opened(), ALREADY_CLOSED, object);
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
				assert(someone.inventory.has_key(object.lock_type), LOCKED, object);
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
	assert(item.valid(), NOTHING_TO_THROW, someone);
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

void Put::commit(Monster & someone, Game & game)
{
	Item & item = someone.inventory.wielded_item();
	assert(item.valid(), NOTHING_TO_PUT, someone);

	item.pos = someone.pos;

	Object & object = find_at(game.level.objects, item.pos + shift);
	if(object.valid() && object.type->drinkable && item.is_emptyable()) {
		assert(item.is_empty(), ALREADY_FULL, item);
		game.messages.fill(someone, item);
		item.make_full();
		return;
	}
	
	if(game.cell_type_at(item.pos + shift).passable) {
		item.pos += shift;
	}
	game.messages.drops(someone, item, game.cell_type_at(item.pos));
	game.level.items.push_back(item);
	someone.inventory.take_wielded_item();
}


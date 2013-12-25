#include "actions.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "cell.h"
#include "util.h"

#define ACTION_ASSERT(condition, expression) \
	do { if(!(condition)) { (expression); return; } } while(0)

void Move::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster.valid(), game.messages.bumps_into(someone, monster));
    Object & object = find_at(game.level.objects, new_pos);
	ACTION_ASSERT(!object.valid() || object.is_passable(), game.messages.bumps_into(someone, object));
	ACTION_ASSERT(game.cell_type_at(new_pos).passable, game.messages.bumps_into(someone, game.cell_type_at(new_pos)));
    someone.pos = new_pos;
}

void Drink::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster.valid(), game.messages.cannot_drink(someone, monster));
    Object & object = find_at(game.level.objects, new_pos);
	ACTION_ASSERT(object.valid() && object.drinkable, game.messages.nothing_to_drink(object));
	game.messages.drinks(someone, object);
	someone.heal_by(1);
}

void Open::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
	ACTION_ASSERT(object.valid() && (object.openable || object.containable), game.messages.nothing_to_open());
    if(object.openable) {
		ACTION_ASSERT(!object.opened, game.messages.already_opened(object));
		if(object.locked) {
			ACTION_ASSERT(someone.inventory.has_key(object.lock_type), game.messages.is_locked(object));
			game.messages.unlocks(someone, object);
			object.locked = false;
		}
		object.opened = true;
		game.messages.opened(someone, object);
    } else if(object.containable) {
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
    ACTION_ASSERT(object.valid() && object.openable, game.messages.nothing_to_close());
    ACTION_ASSERT(object.opened, game.messages.already_closed(object));
    object.opened = false;
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
		if(object.openable && !object.opened) {
			game.messages.swings_at_object(someone, object);
			if(object.locked) {
				ACTION_ASSERT(someone.inventory.has_key(object.lock_type), game.messages.is_locked(object));
				game.messages.unlocks(someone, object);
				object.locked = false;
			}
			object.opened = true;
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
			if(object.containable) {
				game.messages.falls(item, object);
				object.items.push_back(item);
				break;
			} else if(object.drinkable) {
				game.messages.falls_and_lost(item, object);
				break;
			} else if(!object.is_transparent()) {
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
			game.hit(someone, monster, item.damage);
			break;
		}
		item.pos += shift;
	}
}

void Drop::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_drop(someone));
	ACTION_ASSERT(someone.inventory.get_item(slot).valid(), game.messages.no_such_object_in_inventory());
	if(someone.inventory.wields(slot)) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.wears(slot)) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	Item item = someone.inventory.take_item(slot);
	item.pos = someone.pos;
	game.level.items.push_back(item);
	game.messages.drops(someone, item, game.cell_type_at(someone.pos));
}

void Grab::commit(Monster & someone, Game & game)
{
	std::vector<Item>::iterator item_index;
	Item item = find_at(game.level.items, someone.pos, &item_index);
	ACTION_ASSERT(item.valid(), game.messages.nothing_to_pick_up());
	unsigned slot = someone.inventory.insert(item);
	ACTION_ASSERT(slot != Inventory::NOTHING, game.messages.carries_too_much_items(someone));
	game.level.items.erase(item_index);
	game.messages.picks_up(someone, item, game.cell_type_at(someone.pos));
	if(item.quest) {
		game.messages.return_to_gates_with_item();
	}
}

void Wield::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_wield(someone));
	ACTION_ASSERT(someone.inventory.get_item(slot).valid(), game.messages.no_such_object_in_inventory());
	if(someone.inventory.wielded_item().valid()) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.wears(slot)) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	someone.inventory.wield(slot);
	game.messages.wields(someone, someone.inventory.wielded_item());
}

void Unwield::commit(Monster & someone, Game & game)
{
	const Item & item = someone.inventory.wielded_item();
	ACTION_ASSERT(item.valid(), game.messages.wields_nothing(someone));
	game.messages.unwields(someone, item);
	someone.inventory.unwield();
}

void Wear::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_wear(someone));
	const Item & item = someone.inventory.get_item(slot);
	ACTION_ASSERT(item.valid(), game.messages.no_such_object_in_inventory());
	ACTION_ASSERT(item.wearable, game.messages.cannot_be_worn(item));
	if(someone.inventory.wields(slot)) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.worn_item().valid()) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	someone.inventory.wear(slot);
	game.messages.wears(someone, item);
}

void TakeOff::commit(Monster & someone, Game & game)
{
	const Item & item = someone.inventory.worn_item();
	ACTION_ASSERT(item.valid(), game.messages.wears_nothing(someone));
	game.messages.takes_off(someone, item);
	someone.inventory.unwield();
}

void Eat::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_eat(someone));
	Item item = someone.inventory.get_item(slot);
	ACTION_ASSERT(item.valid(), game.messages.no_such_object_in_inventory());
	ACTION_ASSERT(item.edible, game.messages.cannot_be_eaten(item));
	if(someone.inventory.wears(slot)) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	if(someone.inventory.wields(slot)) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	game.messages.eats(someone, item);
	if(item.antidote > 0 && someone.poisoning > 0) {
		someone.poisoning -= item.antidote;
		someone.poisoning = std::max(0, someone.poisoning);
		if(someone.poisoning > 0) {
			game.messages.cures_poisoning_a_little(item);
		} else {
			game.messages.cures_poisoning_fully(item);
		}
	}
	if(item.healing > 0) {
		if(someone.heal_by(item.healing)) {
			game.messages.heals(item, someone);
		}
	}
	someone.inventory.take_item(slot);
}

void GoUp::commit(Monster & someone, Game & game)
{
    Object & object = find_at(game.level.objects, someone.pos);
	ACTION_ASSERT(object.valid() && object.transporting && object.up_destination, game.messages.cannot_go_up(someone));
	if(object.is_exit_up()) {
		const Item & quest_item = someone.inventory.quest_item();
		if(quest_item.valid()) {
			game.messages.win_the_game(someone, quest_item);
			game.state = Game::COMPLETED;
		} else {
			game.messages.sending_to_quest(someone);
		}
	} else {
		game.messages.goes_up(someone);
		game.generate(object.up_destination);
	}
}

void GoDown::commit(Monster & someone, Game & game)
{
    Object & object = find_at(game.level.objects, someone.pos);
	ACTION_ASSERT(object.valid() && object.transporting && object.down_destination, game.messages.cannot_go_down(someone));
	if(object.is_exit_down()) {
		const Item & quest_item = someone.inventory.quest_item();
		if(quest_item.valid()) {
			game.messages.win_the_game(someone, quest_item);
			game.state = Game::COMPLETED;
		} else {
			game.messages.sending_to_quest(someone);
		}
	} else {
		game.messages.goes_down(someone);
		game.generate(object.down_destination);
	}
}

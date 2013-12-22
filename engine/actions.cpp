#include "actions.h"
#include "game.h"
#include "objects.h"
#include "util.h"

#define ACTION_ASSERT(condition, expression) \
	do { if(!(condition)) { (expression); return; } } while(0)

void Move::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
	ACTION_ASSERT(game.level.map.cell(new_pos).passable, game.messages.bumps_into(someone, game.level.map.cell(new_pos)));
    Object & object = find_at(game.level.objects, new_pos);
	if(object.valid()) {
		if(object.openable) {
			if(object.locked) {
				ACTION_ASSERT(object.opened, game.messages.is_locked(object));
			} else {
				ACTION_ASSERT(object.opened, game.messages.is_closed(object));
			}
		}
		ACTION_ASSERT(object.passable, game.messages.bumps_into(someone, object));
	}
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster.valid(), game.messages.bumps_into(someone, monster));
    someone.pos = new_pos;
}

void Drink::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster.valid(), game.messages.cannot_drink(someone, monster));
    Object & object = find_at(game.level.objects, new_pos);
	if(object.valid() && object.drinkable) {
		ACTION_ASSERT(someone.hp < someone.max_hp, game.messages.drinks(someone, object));
		someone.hp += 1;
		someone.hp = std::min(someone.hp, someone.max_hp);
		game.messages.drinks_and_heals(someone, object);
	} else if(object.valid() && object.containable) {
		if(object.items.empty()) {
			game.messages.drink_empty_container(object);
		} else {
			game.messages.drink_container(object);
		}
	} else {
		game.messages.nothing_to_drink();
	}
}

void Open::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
    if(object.valid() && object.openable) {
		ACTION_ASSERT(!object.opened, game.messages.already_opened(object));
		if(object.locked) {
			ACTION_ASSERT(someone.inventory.has_key(object.lock_type), game.messages.is_locked(object));
			game.messages.unlocks(someone, object);
			object.locked = false;
		}
		object.opened = true;
		game.messages.opened(someone, object);
		return;
    }
	ACTION_ASSERT(object.valid() && object.containable, game.messages.nothing_to_open());
	ACTION_ASSERT(!object.items.empty(), game.messages.is_empty(object));
	foreach(Item & item, object.items) {
		item.pos = someone.pos;
		game.level.items.push_back(item);
		game.messages.tooks_up_from(someone, item, object);
	}
	object.items.clear();
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
	ACTION_ASSERT(game.level.map.cell(new_pos).passable, game.messages.hits(someone, game.level.map.cell(new_pos)));
    Object & object = find_at(game.level.objects, new_pos);
	if(object.valid() && object.openable && !object.opened) {
		game.messages.swings_at_object(someone, object);
		if(object.locked) {
			ACTION_ASSERT(someone.inventory.has_key(object.lock_type), game.messages.is_locked(object));
			game.messages.unlocks(someone, object);
			object.locked = false;
		}
		object.opened = true;
		game.messages.opened(someone, object);
		return;
	}
    Monster & monster = find_at(game.level.monsters, new_pos);
	if(monster.valid()) {
		game.hit(someone, monster, someone.damage());
		return;
	}
	ACTION_ASSERT(!object.valid(), game.messages.swings_at_object(someone, object));
    game.messages.swings_at_nothing(someone);
}

void Fire::commit(Monster & someone, Game & game)
{
	Item item = someone.inventory.take_wielded_item();
	ACTION_ASSERT(item.valid(), game.messages.nothing_to_throw(someone));
    item.pos = someone.pos;
	game.messages.throws(someone, item);
	while(true) {
		if(!game.level.map.cell(item.pos + shift).transparent) {
			game.messages.hits(item, game.level.map.cell(item.pos + shift));
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
	game.messages.drops(someone, item, game.level.map.cell(someone.pos));
}

void Grab::commit(Monster & someone, Game & game)
{
	std::vector<Item>::iterator item_index;
	Item item = find_at(game.level.items, someone.pos, &item_index);
	ACTION_ASSERT(item.valid(), game.messages.nothing_to_pick_up());
	unsigned slot = someone.inventory.insert(item);
	ACTION_ASSERT(slot != Inventory::NOTHING, game.messages.carries_too_much_items(someone));
	game.level.items.erase(item_index);
	game.messages.picks_up(someone, item, game.level.map.cell(someone.pos));
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
	if(item.healing > 0 && someone.hp < someone.max_hp) {
		someone.hp += item.healing;
		someone.hp = std::min(someone.hp, someone.max_hp);
		game.messages.heals(item, someone);
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

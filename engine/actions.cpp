#include "actions.h"
#include "game.h"
#include "objects.h"
#include "util.h"
#include <cassert>

#define ACTION_ASSERT(condition, text) \
	do { if(!(condition)) { game.message(text); return; } } while(0)

void smart_move(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
	Point new_pos = someone.pos + shift;
	if(find_at(game.level.monsters, new_pos)) {
		swing(game, someone, shift);
		return;
	}
	Object & object = find_at(game.level.objects, new_pos);
	if(object) {
		if(object.openable && !object.opened) {
			someone.plan.push_front(Control(Control::MOVE, shift));
			open(game, someone, shift);
			return;
		}
		if(object.containable) {
			open(game, someone, shift);
			return;
		}
		if(object.drinkable) {
			drink(game, someone, shift);
			return;
		}
	}
	move(game, someone, shift);
}

void move(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
	Point new_pos = someone.pos + shift;
	ACTION_ASSERT(game.level.map.cell(new_pos).passable, format("{0} bump into the {1}.", someone.name, game.level.map.cell(new_pos).name));
    Object & object = find_at(game.level.objects, new_pos);
	if(object) {
		if(object.openable) {
			if(object.locked) {
				ACTION_ASSERT(object.opened, format("{0} is locked.", object.name));
			} else {
				ACTION_ASSERT(object.opened, format("{0} is closed.", object.name));
			}
		}
		ACTION_ASSERT(object.passable, format("{0} bump into {1}.", someone.name, object.name));
	}
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster, format("{0} bump into {1}.", someone.name, monster.name));
    someone.pos = new_pos;
}

void drink(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level.monsters, new_pos);
	ACTION_ASSERT(!monster, format("It is {1}. {0} is not a vampire to drink that.", someone.name, monster.name));
    Object & object = find_at(game.level.objects, new_pos);
	if(object && object.drinkable) {
		ACTION_ASSERT(someone.hp < someone.max_hp, format("{0} drink from {1}.", someone.name, object.name));
		someone.hp += 1;
		someone.hp = std::min(someone.hp, someone.max_hp);
		game.message(format("{0} drink from {1}. It helps a bit.", someone.name, object.name));
	} else if(object && object.containable) {
		if(object.items.empty()) {
			game.message(format("Unfortunately, {0} is totally empty.", object.name));
		} else {
			game.message(format("Unfortunately, {0} has no water left. But there is something else inside.", object.name));
		}
	} else {
		game.message("There is nothing to drink.");
	}
}

void open(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
    if(object && object.openable) {
		ACTION_ASSERT(!object.opened, format("{0} is already opened.", object.name));
		if(object.locked) {
			ACTION_ASSERT(someone.has_key(object.lock_type), format("{0} is locked.", object.name));
			game.message(format("{0} unlocked the {1}.", someone.name, object.name));
			object.locked = false;
		}
		object.opened = true;
		game.message(format("{0} opened the {1}.", someone.name, object.name));
		return;
    }
	ACTION_ASSERT(object && object.containable, "There is nothing to open there.");
	ACTION_ASSERT(!object.items.empty(), format("{0} is empty.", object.name));
	foreach(Item & item, object.items) {
		item.pos = someone.pos;
		game.level.items.push_back(item);
		game.message(format("{0} took up a {1} from {2}.", someone.name, item.name, object.name));
	}
	object.items.clear();
}

void close(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level.objects, new_pos);
    ACTION_ASSERT(object && object.openable, "There is nothing to close there.");
    ACTION_ASSERT(object.opened, format("{0} is already closed.", object.name));
    object.opened = false;
    game.message(format("{0} closed the {1}.", someone.name, object.name));
}

void swing(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
    Point new_pos = someone.pos + shift;
	ACTION_ASSERT(game.level.map.cell(new_pos).passable, format("{0} hit {1}.", someone.name, game.level.map.cell(new_pos).name));
    Object & object = find_at(game.level.objects, new_pos);
    if(object && object.openable && !object.opened) {
		game.message(format("{0} swing at {1}.", someone.name, object.name));
		open(game, someone, shift);
		return;
    }
    Monster & monster = find_at(game.level.monsters, new_pos);
	if(monster) {
		game.hit(someone, monster, someone.damage());
		return;
	}
	ACTION_ASSERT(!object, format("{0} swing at {1}.", someone.name, object.name));
    game.message(format("{0} swing at nothing.", someone.name));
}

void fire(Game & game, Monster & someone, const Point & shift)
{
	assert(shift);
	Item item = someone.wielded_item();
	ACTION_ASSERT(item, format("{0} have nothing to throw.", someone.name));
	someone.inventory[someone.wielded] = Item();
	someone.wielded = -1;
    item.pos = someone.pos;
	game.message(format("{0} throw {1}.", someone.name, item.name));
	while(true) {
		if(!game.level.map.cell(item.pos + shift).transparent) {
			game.message(format("{0} hit {1}.", item.name, game.level.map.cell(item.pos + shift).name));
			game.level.items.push_back(item);
			break;
		}
		Object & object = find_at(game.level.objects, item.pos + shift);
		if(object) {
			if(object.containable) {
				game.message(format("{0} falls into {1}.", item.name, object.name));
				object.items.push_back(item);
				break;
			} else if(object.drinkable) {
				game.message(format("{0} falls into {1}. Forever lost.", item.name, object.name));
				break;
			} else if(!object.is_transparent()) {
				game.message(format("{0} hit {1}.", item.name, object.name));
				game.level.items.push_back(item);
				break;
			}
		}
		Monster & monster = find_at(game.level.monsters, item.pos + shift);
		if(monster) {
			game.message(format("{0} hits {1}.", item.name, monster.name));
			item.pos += shift;
			game.level.items.push_back(item);
			game.hit(someone, monster, item.damage);
			break;
		}
		item.pos += shift;
	}
}

void drop(Game & game, Monster & someone, int slot)
{
	assert(slot > -1);
	ACTION_ASSERT(!someone.inventory.empty(), format("{0} have nothing to drop.", someone.name));
	ACTION_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	ACTION_ASSERT(someone.inventory[slot], "No such object.");
	if(someone.wielded == slot) {
		unwield(game, someone);
	}
	if(someone.worn == slot) {
		take_off(game, someone);
	}
	Item item = someone.inventory[slot];
	someone.inventory[slot] = Item();
	item.pos = someone.pos;
	game.level.items.push_back(item);
	game.message(format("{0} dropped {1} on the {2}.", someone.name, item.name, game.level.map.cell(someone.pos).name));
}

void grab(Game & game, Monster & someone)
{
	std::vector<Item>::iterator item_index;
	Item item = find_at(game.level.items, someone.pos, &item_index);
	ACTION_ASSERT(item, "Nothing here to pick up.");
	std::vector<Item>::iterator empty_slot;
	for(empty_slot = someone.inventory.begin(); empty_slot != someone.inventory.end(); ++empty_slot) {
		if(!*empty_slot) {
			break;
		}
	}
	if(empty_slot == someone.inventory.end()) {
		ACTION_ASSERT(someone.inventory.size() < 26, format("{0} carry too much items.", someone.name));
		someone.inventory.push_back(item);
	} else {
		*empty_slot = item;
	}
	game.level.items.erase(item_index);
	game.message(format("{0} picked up {1} from the {2}.", someone.name, item.name, game.level.map.cell(someone.pos).name));
	if(item.quest) {
		game.message("Now bring it back to the surface!");
	}
}

void wield(Game & game, Monster & someone, int slot)
{
	assert(slot > -1);
	ACTION_ASSERT(!someone.inventory.empty(), format("{0} have nothing to wield.", someone.name));
	ACTION_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	ACTION_ASSERT(someone.inventory[slot], "No such object.");
	Item item = someone.inventory[slot];
	if(someone.wielded > -1) {
		unwield(game, someone);
	}
	if(someone.worn == slot) {
		take_off(game, someone);
	}
	someone.wielded = slot;
	game.message(format("{0} wields {1}.", someone.name, item.name));
}

void unwield(Game & game, Monster & someone)
{
	ACTION_ASSERT(someone.wielded > -1, format("{0} is wielding nothing.", someone.name));
	Item & item = someone.wielded_item();
	assert(item);
	game.message(format("{0} unwields {1}.", someone.name, item.name));
	someone.wielded = -1;
}

void wear(Game & game, Monster & someone, int slot)
{
	assert(slot > -1);
	ACTION_ASSERT(!someone.inventory.empty(), format("{0} have nothing to wear.", someone.name));
	ACTION_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	ACTION_ASSERT(someone.inventory[slot], "No such object.");
	Item item = someone.inventory[slot];
	ACTION_ASSERT(item.wearable, format("{0} cannot be worn.", item.name));
	if(someone.worn > -1) {
		take_off(game, someone);
	}
	if(someone.wielded == slot) {
		unwield(game, someone);
	}
	someone.worn = slot;
	game.message(format("{0} wear {1}.", someone.name, item.name));
}

void take_off(Game & game, Monster & someone)
{
	ACTION_ASSERT(someone.worn > -1, format("{0} is wearing nothing.", someone.name));
	Item & item = someone.worn_item();
	assert(item);
	game.message(format("{0} takes off {1}.", someone.name, item.name));
	someone.worn = -1;
}

void eat(Game & game, Monster & someone, int slot)
{
	assert(slot > -1);
	ACTION_ASSERT(!someone.inventory.empty(), format("{0} have nothing to eat.", someone.name));
	ACTION_ASSERT(someone.is_valid_slot(slot), "No such object.");
	Item & item = someone.inventory[slot];
	ACTION_ASSERT(item.edible, format("{0} isn't edible.", item.name));
	if(someone.worn == slot) {
		take_off(game, someone);
	}
	if(someone.wielded == slot) {
		unwield(game, someone);
	}
	game.message(format("{0} eats {1}.", someone.name, item.name));
	if(item.antidote > 0 && someone.poisoning > 0) {
		someone.poisoning -= item.antidote;
		someone.poisoning = std::max(0, someone.poisoning);
		if(someone.poisoning > 0) {
			game.message(format("{0} cures poisoning a little.", item.name));
		} else {
			game.message(format("{0} cures poisoning.", item.name));
		}
	}
	if(item.healing > 0 && someone.hp < someone.max_hp) {
		someone.hp += item.healing;
		someone.hp = std::min(someone.hp, someone.max_hp);
		game.message(format("{0} heals {1}.", item.name, someone.name));
	}
	someone.inventory[slot] = Item();
}

void go_up(Game & game, Monster & someone)
{
    Object & object = find_at(game.level.objects, someone.pos);
	ACTION_ASSERT(object && object.transporting && object.up_destination, format("{0} cannot go up from there.", someone.name));
	if(object.is_exit_up()) {
		const Item & quest_item = someone.quest_item();
		if(quest_item) {
			game.message(format("{0} have brought {1} to the surface. Yay! Game if finished.", someone.name, quest_item.name));
			game.state = Game::COMPLETED;
		} else {
			game.message(format("{0} must complete mission in order to go back to the surface.", someone.name));
		}
	} else {
		game.message(format("{0} goes up.", someone.name));
		game.generate(object.up_destination);
	}
}

void go_down(Game & game, Monster & someone)
{
    Object & object = find_at(game.level.objects, someone.pos);
	ACTION_ASSERT(object && object.transporting && object.down_destination, format("{0} cannot go down from there.", someone.name));
	if(object.is_exit_down()) {
		const Item & quest_item = someone.quest_item();
		if(quest_item) {
			game.message(format("{0} have brought {1} to the surface. Yay! Game if finished.", someone.name, quest_item.name));
			game.state = Game::COMPLETED;
		} else {
			game.message(format("{0} must complete mission in order to go back to the surface.", someone.name));
		}
	} else {
		game.message(format("{0} goes down.", someone.name));
		game.generate(object.down_destination);
	}
}

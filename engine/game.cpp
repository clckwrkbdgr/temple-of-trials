#include "game.h"
#include <map>
#include <cstdlib>
#include <cassert>
#include <cmath>

#define GAME_ASSERT(condition, text) \
	do { if(!(condition)) { message(text); return; } } while(0)

Control::Control(int control_value)
	: control(control_value), slot(-1)
{
}

Control::Control(int control_value, const Point & control_direction)
	: control(control_value), direction(control_direction), slot(-1)
{
}

Control::Control(int control_value, int control_slot)
	: control(control_value), slot(control_slot)
{
}


Game::Game(LevelGenerator * level_generator)
	: log_messages(false), current_level(0), generator(level_generator),
	done(false), player_died(false), completed(false), turn_ended(false),
	turns(0)
{
}

void Game::run(ControllerFactory controller_factory)
{
	while(!done) {
		turn_ended = false;
		foreach(Monster & monster, level.monsters) {
			if(monster.is_dead()) {
				continue;
			}
			level.invalidate_fov(monster);
			Controller controller = controller_factory(monster.ai);
			if(!controller) {
				log(format("No controller found for AI #{0}!", monster.ai));
				continue;
			}
			Control control = controller(monster, *this);
			switch(control.control) {
				case Control::SMART_MOVE: smart_move(monster, control.direction); break;
				case Control::MOVE: move(monster, control.direction); break;
				case Control::OPEN: open(monster, control.direction); break;
				case Control::CLOSE: close(monster, control.direction); break;
				case Control::SWING: swing(monster, control.direction); break;
				case Control::FIRE: fire(monster, control.direction); break;
				case Control::DRINK: drink(monster, control.direction); break;
				case Control::GRAB: grab(monster); break;
				case Control::DROP: drop(monster, control.slot); break;
				case Control::WIELD: wield(monster, control.slot); break;
				case Control::UNWIELD: unwield(monster); break;
				case Control::WEAR: wear(monster, control.slot); break;
				case Control::TAKE_OFF: take_off(monster); break;
				case Control::EAT: eat(monster, control.slot); break;
				case Control::GO_UP: go_up(monster); break;
				case Control::GO_DOWN: go_down(monster); break;
				case Control::WAIT: break;
				default: log("Unknown control: {0}", control.control); break;
			}
			if(turn_ended) {
				break;
			}

			process_environment(monster);

			if(done) {
				break;
			}
		}
		level.erase_dead_monsters();
		++turns;
	}
}

void Game::generate(int level_index)
{
	if(current_level != 0) {
		saved_levels[current_level] = level;
	}

	Monster player = level.get_player();
	if(saved_levels.count(level_index) > 0) {
		level = saved_levels[level_index];
		saved_levels.erase(level_index);
	} else if(generator) {
		generator->generate(level, level_index);
	} else {
		return;
	}
	if(player) {
		player.pos = level.get_player().pos;
		level.get_player() = player;
	}
	current_level = level_index;
	turn_ended = true;
}

void Game::message(std::string text)
{
	if(text.empty()) {
		return;
	}
	text[0] = toupper(text[0]);
	messages.push_back(text);
	if(log_messages) {
		log("Message: " + text);
	}
}

void Game::process_environment(Monster & someone)
{
	if(level.map.cell(someone.pos).hurts) {
		message("It hurts!");
		hurt(someone, 1);
	}
	Object & trap = find_at(level.traps, someone.pos);
	if(trap) {
		if(trap.items.empty()) {
			message(format("{0} is already triggered.", trap.name));
		} else {
			message(format("{0} trigger the {1}.", someone.name, trap.name));
			trap.items.back().pos = trap.pos;
			level.items.push_back(trap.items.back());
			trap.items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		message(format("{0} is poisoned.", someone.name));
		--someone.poisoning;
		hurt(someone, 1, true);
	} else if(someone.poisoning < 0) {
		someone.poisoning = 0;
	}
}

void Game::die(Monster & someone)
{
	foreach(Item & item, someone.inventory) {
		item.pos = someone.pos;
		level.items.push_back(item);
		message(format("{0} drops {1}.", someone.name, item.name));
	}
	someone.inventory.clear();
	if(someone.faction == Monster::PLAYER) {
		message("You died.");
		done = true;
		player_died = true;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.worn_item().defence);
	if(!someone.godmode) {
		someone.hp -= received_damage;
	}
	GAME_ASSERT(someone.is_dead(), format("{0} loses {1} hp.", someone.name, received_damage));
	message(format("{0} loses {1} hp and dies.", someone.name, received_damage));
	die(someone);
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.worn_item().defence;
	if(!other.godmode) {
		other.hp -= received_damage;
	}
	if(someone.poisonous) {
		message(format("{0} poisons {1}.", someone.name, other.name));
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	GAME_ASSERT(other.is_dead(), format("{0} hit {1} for {2} hp.", someone.name, other.name, received_damage));
	message(format("{0} hit {1} for {2} hp and kills it.", someone.name, other.name, received_damage));
	die(other);
}

void Game::smart_move(Monster & someone, const Point & shift)
{
	assert(shift);
	Point new_pos = someone.pos + shift;
	Door & door = find_at(level.doors, new_pos);
	if(door && !door.opened) {
		someone.plan.push_front(Control(Control::MOVE, shift));
		open(someone, shift);
		return;
	}
	Object & object = find_at(level.objects, new_pos);
	if(object && object.containable) {
		open(someone, shift);
		return;
	}
	if(object && object.drinkable) {
		drink(someone, shift);
		return;
	}
	if(find_at(level.monsters, new_pos)) {
		swing(someone, shift);
		return;
	}
	move(someone, shift);
}

void Game::move(Monster & someone, const Point & shift)
{
	assert(shift);
	Point new_pos = someone.pos + shift;
	GAME_ASSERT(level.map.cell(new_pos).passable, format("{0} bump into the {1}.", someone.name, level.map.cell(new_pos).name));
    Door & door = find_at(level.doors, new_pos);
	GAME_ASSERT(!door || door.opened, format("{0} is closed.", door.name));
    Object & object = find_at(level.objects, new_pos);
	GAME_ASSERT(!object, format("{0} bump into {1}.", someone.name, object.name));
    Monster & monster = find_at(level.monsters, new_pos);
	GAME_ASSERT(!monster, format("{0} bump into {1}.", someone.name, monster.name));
    someone.pos = new_pos;
}

void Game::drink(Monster & someone, const Point & shift)
{
	assert(shift);
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(level.monsters, new_pos);
	GAME_ASSERT(!monster, format("It is {1}. {0} is not a vampire to drink that.", someone.name, monster.name));
    Object & object = find_at(level.objects, new_pos);
	if(object.drinkable) {
		GAME_ASSERT(someone.hp < someone.max_hp, format("{0} drink from {1}.", someone.name, object.name));
		someone.hp += 1;
		someone.hp = std::min(someone.hp, someone.max_hp);
		message(format("{0} drink from {1}. It helps a bit.", someone.name, object.name));
	} else if(object.containable) {
		if(object.items.empty()) {
			message(format("Unfortunately, {0} is totally empty.", object.name));
		} else {
			message(format("Unfortunately, {0} has no water left. But there is something else inside.", object.name));
		}
	} else {
		message("There is nothing to drink.");
	}
}

void Game::open(Monster & someone, const Point & shift)
{
	assert(shift);
    Point new_pos = someone.pos + shift;
    Door & door = find_at(level.doors, new_pos);
    if(door) {
		GAME_ASSERT(!door.opened, format("{0} is already opened.", door.name));
		door.opened = true;
		message(format("{0} opened the {1}.", someone.name, door.name));
		return;
    }
	Object & object = find_at(level.objects, new_pos);
	GAME_ASSERT(object && object.containable, "There is nothing to open there.");
	GAME_ASSERT(!object.items.empty(), format("{0} is empty.", object.name));
	foreach(Item & item, object.items) {
		item.pos = someone.pos;
		level.items.push_back(item);
		message(format("{0} took up a {1} from {2}.", someone.name, item.name, object.name));
	}
	object.items.clear();
}

void Game::close(Monster & someone, const Point & shift)
{
	assert(shift);
    Point new_pos = someone.pos + shift;
    Door & door = find_at(level.doors, new_pos);
    GAME_ASSERT(door, "There is nothing to close there.");
    GAME_ASSERT(door.opened, format("{0} is already closed.", door.name));
    door.opened = false;
    message(format("{0} closed the {1}.", someone.name, door.name));
}

void Game::swing(Monster & someone, const Point & shift)
{
	assert(shift);
    Point new_pos = someone.pos + shift;
	GAME_ASSERT(level.map.cell(new_pos).passable, format("{0} hit {1}.", someone.name, level.map.cell(new_pos).name));
    Door & door = find_at(level.doors, new_pos);
    if(door && !door.opened) {
		message(format("{0} swing at {1}.", someone.name, door.name));
		open(someone, shift);
		return;
    }
    Monster & monster = find_at(level.monsters, new_pos);
	if(monster) {
		hit(someone, monster, someone.damage());
		return;
	}
    Object & object = find_at(level.objects, new_pos);
	GAME_ASSERT(!object, format("{0} swing at {1}.", someone.name, object.name));
    message(format("{0} swing at nothing.", someone.name));
}

void Game::fire(Monster & someone, const Point & shift)
{
	assert(shift);
	Item item = someone.wielded_item();
	GAME_ASSERT(item, format("{0} have nothing to throw.", someone.name));
	someone.inventory[someone.wielded] = Item();
	someone.wielded = -1;
    item.pos = someone.pos;
	message(format("{0} throw {1}.", someone.name, item.name));
	while(true) {
		if(!level.map.cell(item.pos + shift).transparent) {
			message(format("{0} hit {1}.", item.name, level.map.cell(item.pos + shift).name));
			level.items.push_back(item);
			break;
		}
		Door & door = find_at(level.doors, item.pos + shift);
		if(door && !door.opened) {
			message(format("{0} hit {1}.", item.name, door.name));
			level.items.push_back(item);
			break;
		}
		Object & object = find_at(level.objects, item.pos + shift);
		if(object) {
			if(object.containable) {
				message(format("{0} falls into {1}.", item.name, object.name));
				object.items.push_back(item);
				break;
			} else {
				message(format("{0} falls into {1}. Forever lost.", item.name, object.name));
				break;
			}
		}
		Monster & monster = find_at(level.monsters, item.pos + shift);
		if(monster) {
			message(format("{0} hits {1}.", item.name, monster.name));
			item.pos += shift;
			level.items.push_back(item);
			hit(someone, monster, item.damage);
			break;
		}
		item.pos += shift;
	}
}

void Game::drop(Monster & someone, int slot)
{
	assert(slot > -1);
	GAME_ASSERT(!someone.inventory.empty(), format("{0} have nothing to drop.", someone.name));
	GAME_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	GAME_ASSERT(someone.inventory[slot], "No such object.");
	if(someone.wielded == slot) {
		unwield(someone);
	}
	if(someone.worn == slot) {
		take_off(someone);
	}
	Item item = someone.inventory[slot];
	someone.inventory[slot] = Item();
	item.pos = someone.pos;
	level.items.push_back(item);
	message(format("{0} dropped {1} on the {2}.", someone.name, item.name, level.map.cell(someone.pos).name));
}

void Game::grab(Monster & someone)
{
	std::vector<Item>::iterator item_index;
	Item item = find_at(level.items, someone.pos, &item_index);
	GAME_ASSERT(item, "Nothing here to pick up.");
	std::vector<Item>::iterator empty_slot;
	for(empty_slot = someone.inventory.begin(); empty_slot != someone.inventory.end(); ++empty_slot) {
		if(!*empty_slot) {
			break;
		}
	}
	if(empty_slot == someone.inventory.end()) {
		GAME_ASSERT(someone.inventory.size() < 26, format("{0} carry too much items.", someone.name));
		someone.inventory.push_back(item);
	} else {
		*empty_slot = item;
	}
	level.items.erase(item_index);
	message(format("{0} picked up {1} from the {2}.", someone.name, item.name, level.map.cell(someone.pos).name));
	if(item.quest) {
		message("Now bring it back to the surface!");
	}
}

void Game::wield(Monster & someone, int slot)
{
	assert(slot > -1);
	GAME_ASSERT(!someone.inventory.empty(), format("{0} have nothing to wield.", someone.name));
	GAME_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	GAME_ASSERT(someone.inventory[slot], "No such object.");
	Item item = someone.inventory[slot];
	if(someone.wielded > -1) {
		unwield(someone);
	}
	if(someone.worn == slot) {
		take_off(someone);
	}
	someone.wielded = slot;
	message(format("{0} wields {1}.", someone.name, item.name));
}

void Game::unwield(Monster & someone)
{
	GAME_ASSERT(someone.wielded > -1, format("{0} is wielding nothing.", someone.name));
	Item & item = someone.wielded_item();
	assert(item);
	message(format("{0} unwields {1}.", someone.name, item.name));
	someone.wielded = -1;
}

void Game::wear(Monster & someone, int slot)
{
	assert(slot > -1);
	GAME_ASSERT(!someone.inventory.empty(), format("{0} have nothing to wear.", someone.name));
	GAME_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	GAME_ASSERT(someone.inventory[slot], "No such object.");
	Item item = someone.inventory[slot];
	GAME_ASSERT(item.wearable, format("{0} cannot be worn.", item.name));
	if(someone.worn > -1) {
		take_off(someone);
	}
	if(someone.wielded == slot) {
		unwield(someone);
	}
	someone.worn = slot;
	message(format("{0} wear {1}.", someone.name, item.name));
}

void Game::take_off(Monster & someone)
{
	GAME_ASSERT(someone.worn > -1, format("{0} is wearing nothing.", someone.name));
	Item & item = someone.worn_item();
	assert(item);
	message(format("{0} takes off {1}.", someone.name, item.name));
	someone.worn = -1;
}

void Game::eat(Monster & someone, int slot)
{
	assert(slot > -1);
	GAME_ASSERT(!someone.inventory.empty(), format("{0} have nothing to eat.", someone.name));
	GAME_ASSERT(slot < int(someone.inventory.size()), "No such object.");
	Item & item = someone.inventory[slot];
	GAME_ASSERT(item, "No such object.");
	GAME_ASSERT(item.edible, format("{0} isn't edible.", item.name));
	if(someone.worn == slot) {
		take_off(someone);
	}
	if(someone.wielded == slot) {
		unwield(someone);
	}
	message(format("{0} eats {1}.", someone.name, item.name));
	if(item.antidote > 0 && someone.poisoning > 0) {
		someone.poisoning -= item.antidote;
		someone.poisoning = std::max(0, someone.poisoning);
		if(someone.poisoning > 0) {
			message(format("{0} cures poisoning a little.", item.name));
		} else {
			message(format("{0} cures poisoning.", item.name));
		}
	}
	if(item.healing > 0 && someone.hp < someone.max_hp) {
		someone.hp += item.healing;
		someone.hp = std::min(someone.hp, someone.max_hp);
		message(format("{0} heals {1}.", item.name, someone.name));
	}
	someone.inventory[slot] = Item();
}

void Game::go_up(Monster & someone)
{
    Object & object = find_at(level.objects, someone.pos);
	GAME_ASSERT(object && object.transporting && object.up_destination, format("{0} cannot go up from there.", someone.name));
	if(object.up_destination < 0) {
		foreach(const Item & item, someone.inventory) {
			if(item.quest) {
				message(format("{0} have brought {1} to the surface. Yay! Game if finished.", someone.name, item.name));
				done = true;
				completed = true;
				return;
			}
		}
		message(format("{0} must complete mission in order to go back to the surface.", someone.name));
		return;
	}
	message(format("{0} goes up.", someone.name));
	generate(object.up_destination);
}

void Game::go_down(Monster & someone)
{
    Object & object = find_at(level.objects, someone.pos);
	GAME_ASSERT(object && object.transporting && object.down_destination, format("{0} cannot go down from there.", someone.name));
	if(object.down_destination < 0) {
		foreach(const Item & item, someone.inventory) {
			if(item.quest) {
				message(format("{0} have brought {1} to the surface. Yay! Game if finished.", someone.name, item.name));
				done = true;
				completed = true;
				return;
			}
		}
		message(format("{0} must complete mission in order to go back to the surface.", someone.name));
		return;
	}
	message(format("{0} goes down.", someone.name));
	generate(object.down_destination);
}


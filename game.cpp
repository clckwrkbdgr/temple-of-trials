#include "ai.h"
#include "game.h"
#include <map>
#include <cstdlib>

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


Game::Game()
	: map(1, 1, Cell()), done(false), player_died(false), turns(0)
{
}

Point Game::find_random_free_cell() const
{
	int counter = map.get_width() * map.get_height();
	while(--counter > 0) {
		Point new_pos = Point(rand() % map.get_width(), rand() % map.get_height());
		if(!map.is_passable(new_pos)) {
			continue;
		}
		if(find_at(doors, new_pos)) {
			continue;
		}
		if(find_at(monsters, new_pos)) {
			continue;
		}
		return new_pos;
	}
	return Point();
}

const Monster & Game::getPlayer() const
{
	foreach(const Monster & monster, monsters) {
		if(monster.ai == AI::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	return empty;
}

void Game::message(std::string text)
{
	if(text.empty()) {
		return;
	}
	text[0] = toupper(text[0]);
	messages.push_back(text);
	log("Message: " + text);
}

void Game::move(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
	Point new_pos = someone.pos + shift;
	if(!map.is_passable(new_pos)) {
		message(format("{0} bump into the wall.", someone.name));
		return;
	}
    Door & door = find_at(doors, new_pos);
	if(door && !door.opened) {
		message("Door is closed.");
		return;
	}
    Container & container = find_at(containers, new_pos);
	if(container) {
		message(format("{0} bump into {1}.", someone.name, container.name));
		return;
	}
    Monster & monster = find_at(monsters, new_pos);
	if(monster) {
		message(format("{0} bump into {1}.", someone.name, monster.name));
		return;
	}
    someone.pos = new_pos;
}

void Game::open(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(door) {
		if(door.opened) {
			message("Door is already opened.");
			return;
		}
		door.opened = true;
		message(format("{0} opened the door.", someone.name));
		return;
    }
	Container & container = find_at(containers, new_pos);
	if(container) {
		if(container.items.empty()) {
			message(format("{0} is empty.", container.name));
			return;
		}
		foreach(Item & item, container.items) {
			item.pos = someone.pos;
			items.push_back(item);
			message(format("{0} took up a {1} from {2}.", someone.name, item.name, container.name));
		}
		container.items.clear();
		return;
	}
	message("There is nothing to open there.");
}

void Game::close(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(!door) {
        message("There is nothing to close there.");
        return;
    }
    if(!door.opened) {
        message("Door is already closed.");
        return;
    }
    door.opened = false;
    message(format("{0} closed the door.", someone.name));
}

void Game::swing(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(door && !door.opened) {
		message(format("{0} swing at door.", someone.name));
		open(someone, shift);
		return;
    }
    Monster & monster = find_at(monsters, new_pos);
	if(monster) {
		monster.hp -= someone.damage();
		message(format("{0} hit {1} for {2} hp.", someone.name, monster.name, someone.damage()));
		if(monster.is_dead()) {
			message(format("{0} kill {1}.", someone.name, monster.name));
			foreach(Item & item, monster.inventory) {
				item.pos = monster.pos;
				items.push_back(item);
				message(format("{0} drops {1}.", monster.name, item.name));
			}
			monster.inventory.clear();
			if(monster.ai == AI::PLAYER) {
				message("You died.");
				done = true;
				player_died = true;
			}
		}
		return;
	}
    Container & container = find_at(containers, new_pos);
	if(container) {
		message(format("{0} swing at {1}.", someone.name, container.name));
		return;
	}
	if(!map.is_passable(new_pos)) {
		message(format("{0} hit wall.", someone.name));
		return;
	}
    message(format("{0} swing at nothing.", someone.name));
}

void Game::drop(Monster & someone, int slot)
{
	if(slot < 0) {
		return;
	}
	if(someone.inventory.empty()) {
		message(format("{0} have nothing to drop.", someone.name));
		return;
	}
	if(int(someone.inventory.size()) < slot) {
		message("No such object.");
		return;
	}
	if(!someone.inventory[slot]) {
		message("No such object.");
		return;
	}
	if(someone.wielded == slot) {
		unwield(someone);
	}
	if(someone.worn == slot) {
		take_off(someone);
	}
	Item item = someone.inventory[slot];
	someone.inventory[slot] = Item();
	item.pos = someone.pos;
	items.push_back(item);
	message(format("{0} dropped {1} on the floor.", someone.name, item.name));
}

void Game::grab(Monster & someone)
{
	std::vector<Item>::iterator item_index;
	Item item = find_at(items, someone.pos, &item_index);
	if(!item) {
		message("Nothing here to pick up.");
		return;
	}
	std::vector<Item>::iterator empty_slot;
	for(empty_slot = someone.inventory.begin(); empty_slot != someone.inventory.end(); ++empty_slot) {
		if(!*empty_slot) {
			break;
		}
	}
	if(empty_slot == someone.inventory.end()) {
		if(someone.inventory.size() >= 26) {
			message(format("{0} carry too much items.", someone.name));
			return;
		}
		someone.inventory.push_back(item);
	} else {
		*empty_slot = item;
	}
	items.erase(item_index);
	message(format("{0} picked up {1} from the floor.", someone.name, item.name));
}

void Game::wield(Monster & someone, int slot)
{
	if(slot < 0) {
		return;
	}
	if(someone.inventory.empty()) {
		message(format("{0} have nothing to wield.", someone.name));
		return;
	}
	if(int(someone.inventory.size()) < slot) {
		message("No such object.");
		return;
	}
	if(!someone.inventory[slot]) {
		message("No such object.");
		return;
	}
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
	if(someone.wielded < 0) {
		message(format("{0} is wielding nothing.", someone.name));
		return;
	}
	if(someone.wielded >= int(someone.inventory.size())) {
		log("{0} was wielding incorrect slot: {1}", someone.name, someone.wielded);
		someone.wielded = -1;
		return;
	}
	Item & item = someone.inventory[someone.wielded];
	if(!item) {
		log("{0} was wielding empty slot: {1}", someone.name, someone.wielded);
		someone.wielded = -1;
		return;
	}
	message(format("{0} unwields {1}.", someone.name, item.name));
	someone.wielded = -1;
}

void Game::wear(Monster & someone, int slot)
{
	if(slot < 0) {
		return;
	}
	if(someone.inventory.empty()) {
		message(format("{0} have nothing to wear.", someone.name));
		return;
	}
	if(int(someone.inventory.size()) < slot) {
		message("No such object.");
		return;
	}
	if(!someone.inventory[slot]) {
		message("No such object.");
		return;
	}
	Item item = someone.inventory[slot];
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
	if(someone.worn < 0) {
		message(format("{0} is wearing nothing.", someone.name));
		return;
	}
	if(someone.worn >= int(someone.inventory.size())) {
		log("{0} was wearing incorrect slot: {1}", someone.name, someone.worn);
		someone.worn = -1;
		return;
	}
	Item & item = someone.inventory[someone.worn];
	if(!item) {
		log("{0} was wearing empty slot: {1}", someone.name, someone.worn);
		someone.worn = -1;
		return;
	}
	message(format("{0} takes off {1}.", someone.name, item.name));
	someone.worn = -1;
}


#include "ai.h"
#include "game.h"
#include <map>
#include <cstdlib>

void game_assert(bool condition, const std::string & message)
{
	if(!condition) {
		throw Game::Message(message);
	}
}

void game_assert(bool condition)
{
	if(!condition) {
		throw Game::Message();
	}
}


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
	: map(1, 1), done(false), player_died(false), turns(0)
{
}

Point Game::find_random_free_cell() const
{
	int counter = map.width * map.height;
	while(--counter > 0) {
		Point new_pos = Point(rand() % map.width, rand() % map.height);
		if(!map.cell(new_pos).passable) {
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

void Game::process_environment(Monster & someone)
{
	if(map.cell(someone.pos).hurts) {
		message("It hurts!");
		hurt(someone, 1);
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
		items.push_back(item);
		message(format("{0} drops {1}.", someone.name, item.name));
	}
	someone.inventory.clear();
	if(someone.ai == AI::PLAYER) {
		message("You died.");
		done = true;
		player_died = true;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.worn_item().defence);
	someone.hp -= received_damage;
	game_assert(someone.is_dead(), format("{0} loses {1} hp.", someone.name, received_damage));
	message(format("{0} loses {1} hp and dies.", someone.name, received_damage));
	die(someone);
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.worn_item().defence;
	other.hp -= received_damage;
	if(someone.poisonous) {
		message(format("{0} poisons {1}.", someone.name, other.name));
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	game_assert(other.is_dead(), format("{0} hit {1} for {2} hp.", someone.name, other.name, received_damage));
	message(format("{0} hit {1} for {2} hp and kills it.", someone.name, other.name, received_damage));
	die(other);
}

void Game::move(Monster & someone, const Point & shift)
{
	game_assert(shift);
	Point new_pos = someone.pos + shift;
	game_assert(map.cell(new_pos).passable, format("{0} bump into the wall.", someone.name));
    Door & door = find_at(doors, new_pos);
	game_assert(!door || door.opened, "Door is closed.");
    Container & container = find_at(containers, new_pos);
	game_assert(!container, format("{0} bump into {1}.", someone.name, container.name));
    Fountain & fountain = find_at(fountains, new_pos);
	game_assert(!fountain, format("{0} bump into {1}.", someone.name, fountain.name));
    Monster & monster = find_at(monsters, new_pos);
	game_assert(!monster, format("{0} bump into {1}.", someone.name, monster.name));
    someone.pos = new_pos;
}

void Game::drink(Monster & someone, const Point & shift)
{
	game_assert(shift);
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(monsters, new_pos);
	game_assert(!monster, format("It is {1}. {0} is not a vampire to drink that.", someone.name, monster.name));
    Container & container = find_at(containers, new_pos);
	game_assert(
			!(container && !container.items.empty()),
			format("Unfortunately, {0} has no water left. But there is something else inside.", container.name)
			);
	game_assert(!container, format("Unfortunately, {0} is totally empty.", container.name));
	Fountain & fountain = find_at(fountains, new_pos);
	game_assert(fountain, "There is nothing to drink.");
	game_assert(someone.hp < someone.max_hp, format("{0} drink from {1}.", someone.name, fountain.name));
	someone.hp += 1;
	someone.hp = std::min(someone.hp, someone.max_hp);
	message(format("{0} drink from {1}. It helps a bit.", someone.name, fountain.name));
}

void Game::open(Monster & someone, const Point & shift)
{
	game_assert(shift);
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(door) {
		game_assert(!door.opened, "Door is already opened.");
		door.opened = true;
		message(format("{0} opened the door.", someone.name));
		return;
    }
	Container & container = find_at(containers, new_pos);
	game_assert(container, "There is nothing to open there.");
	game_assert(!container.items.empty(), format("{0} is empty.", container.name));
	foreach(Item & item, container.items) {
		item.pos = someone.pos;
		items.push_back(item);
		message(format("{0} took up a {1} from {2}.", someone.name, item.name, container.name));
	}
	container.items.clear();
}

void Game::close(Monster & someone, const Point & shift)
{
	game_assert(shift);
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    game_assert(door, "There is nothing to close there.");
    game_assert(door.opened, "Door is already closed.");
    door.opened = false;
    message(format("{0} closed the door.", someone.name));
}

void Game::swing(Monster & someone, const Point & shift)
{
	game_assert(shift);
    Point new_pos = someone.pos + shift;
	game_assert(map.cell(new_pos).passable, format("{0} hit wall.", someone.name));
    Door & door = find_at(doors, new_pos);
    if(door && !door.opened) {
		message(format("{0} swing at door.", someone.name));
		open(someone, shift);
		return;
    }
    Monster & monster = find_at(monsters, new_pos);
	if(monster) {
		hit(someone, monster, someone.damage());
		return;
	}
    Container & container = find_at(containers, new_pos);
	game_assert(!container, format("{0} swing at {1}.", someone.name, container.name));
    message(format("{0} swing at nothing.", someone.name));
}

void Game::fire(Monster & someone, const Point & shift)
{
	game_assert(shift);
	Item item = someone.wielded_item();
	game_assert(item, format("{0} have nothing to throw.", someone.name));
	someone.inventory[someone.wielded] = Item();
	someone.wielded = -1;
    item.pos = someone.pos;
	message(format("{0} throw {1}.", someone.name, item.name));
	while(true) {
		if(!map.cell(item.pos + shift).transparent) {
			message(format("{0} hit wall.", item.name));
			items.push_back(item);
			break;
		}
		Door & door = find_at(doors, item.pos + shift);
		if(door && !door.opened) {
			message(format("{0} hit door.", item.name));
			items.push_back(item);
			break;
		}
		Container & container = find_at(containers, item.pos + shift);
		if(container) {
			message(format("{0} falls into {1}.", item.name, container.name));
			container.items.push_back(item);
			break;
		}
		Fountain & fountain = find_at(fountains, item.pos + shift);
		if(fountain) {
			message(format("{0} falls into {1}. Forever lost.", item.name, fountain.name));
			break;
		}
		Monster & monster = find_at(monsters, item.pos + shift);
		if(monster) {
			message(format("{0} hits {1}.", item.name, monster.name));
			item.pos += shift;
			hit(someone, monster, item.damage);
			items.push_back(item);
			break;
		}
		item.pos += shift;
	}
}

void Game::drop(Monster & someone, int slot)
{
	game_assert(slot > -1);
	game_assert(!someone.inventory.empty(), format("{0} have nothing to drop.", someone.name));
	game_assert(slot < int(someone.inventory.size()), "No such object.");
	game_assert(someone.inventory[slot], "No such object.");
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
	game_assert(item, "Nothing here to pick up.");
	std::vector<Item>::iterator empty_slot;
	for(empty_slot = someone.inventory.begin(); empty_slot != someone.inventory.end(); ++empty_slot) {
		if(!*empty_slot) {
			break;
		}
	}
	if(empty_slot == someone.inventory.end()) {
		game_assert(someone.inventory.size() < 26, format("{0} carry too much items.", someone.name));
		someone.inventory.push_back(item);
	} else {
		*empty_slot = item;
	}
	items.erase(item_index);
	message(format("{0} picked up {1} from the floor.", someone.name, item.name));
	if(item.quest) {
		message("Now bring it back to the surface!");
	}
}

void Game::wield(Monster & someone, int slot)
{
	game_assert(slot > -1);
	game_assert(!someone.inventory.empty(), format("{0} have nothing to wield.", someone.name));
	game_assert(slot < int(someone.inventory.size()), "No such object.");
	game_assert(someone.inventory[slot], "No such object.");
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
	game_assert(someone.wielded > -1, format("{0} is wielding nothing.", someone.name));
	Item & item = someone.wielded_item();
	game_assert(item);
	message(format("{0} unwields {1}.", someone.name, item.name));
	someone.wielded = -1;
}

void Game::wear(Monster & someone, int slot)
{
	game_assert(slot > -1);
	game_assert(!someone.inventory.empty(), format("{0} have nothing to wear.", someone.name));
	game_assert(slot < int(someone.inventory.size()), "No such object.");
	game_assert(someone.inventory[slot], "No such object.");
	Item item = someone.inventory[slot];
	game_assert(item.wearable, format("{0} cannot be worn.", item.name));
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
	game_assert(someone.worn > -1, format("{0} is wearing nothing.", someone.name));
	Item & item = someone.worn_item();
	game_assert(item);
	message(format("{0} takes off {1}.", someone.name, item.name));
	someone.worn = -1;
}

void Game::eat(Monster & someone, int slot)
{
	game_assert(slot > -1);
	game_assert(!someone.inventory.empty(), format("{0} have nothing to eat.", someone.name));
	game_assert(slot < int(someone.inventory.size()), "No such object.");
	Item & item = someone.inventory[slot];
	game_assert(item, "No such object.");
	game_assert(item.edible, format("{0} isn't edible.", item.name));
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


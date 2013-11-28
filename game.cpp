#include "ai.h"
#include "game.h"
#include <map>
#include <cstdlib>
#include <cmath>

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


Game::MapPassabilityDetector::MapPassabilityDetector(const Game & _game)
	: game(_game)
{
}

bool Game::MapPassabilityDetector::is_passable(int x, int y) const
{
	Point new_pos(x, y);
	if(!game.map.cell(new_pos).passable) {
		return false;
	}
    const Door & door = find_at(game.doors, new_pos);
	if(door && !door.opened) {
		return false;
	}
    const Container & container = find_at(game.containers, new_pos);
	if(container) {
		return false;
	}
    const Fountain & fountain = find_at(game.fountains, new_pos);
	if(fountain) {
		return false;
	}
    const Monster & monster = find_at(game.monsters, new_pos);
	if(monster) {
		return false;
	}
	return true;
}


Game::Game(MapGenerator * map_generator)
	: map(1, 1), current_level(0), generator(map_generator),
	done(false), player_died(false), completed(false), turn_ended(false),
	turns(0)
{
}

void Game::generate(int level)
{
	if(generator) {
		generator->generate(*this, level);
		current_level = level;
		turn_ended = true;
	}
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

bool Game::transparent(int x, int y) const
{
	foreach(const Door & door, doors) {
		if(door.pos == Point(x, y) && !door.opened) {
			return false;
		}
	}
	return map.cell(x, y).transparent;
}

int Game::get_sprite_at(int x, int y) const
{
	return get_sprite_at(Point(x, y));
}

int Game::get_sprite_at(const Point & pos) const
{
	foreach(const Monster & monster, monsters) {
		if(monster.pos == pos) {
			return monster.sprite;
		}
	}
	foreach(const Item & item, items) {
		if(item.pos == pos) {
			return item.sprite;
		}
	}
	foreach(const Container & container, containers) {
		if(container.pos == pos) {
			return container.sprite;
		}
	}
	foreach(const Fountain & fountain, fountains) {
		if(fountain.pos == pos) {
			return fountain.sprite;
		}
	}
	foreach(const Stairs & stair, stairs) {
		if(stair.pos == pos) {
			return stair.sprite;
		}
	}
	foreach(const Door & door, doors) {
		if(door.pos == pos) {
			return door.sprite();
		}
	}
	return map.cell(pos).sprite;
}

std::string Game::name_at(const Point & pos) const
{
	foreach(const Monster & monster, monsters) {
		if(monster.pos == pos) {
			return monster.name;
		}
	}
	foreach(const Item & item, items) {
		if(item.pos == pos) {
			return item.name;
		}
	}
	foreach(const Container & container, containers) {
		if(container.pos == pos) {
			return container.name;
		}
	}
	foreach(const Fountain & fountain, fountains) {
		if(fountain.pos == pos) {
			return fountain.name;
		}
	}
	foreach(const Stairs & stair, stairs) {
		if(stair.pos == pos) {
			return stair.name;
		}
	}
	foreach(const Door & door, doors) {
		if(door.pos == pos) {
			return door.name;
		}
	}
	return map.cell(pos).name;
}

void Game::invalidate_fov(Monster & monster)
{
	for(unsigned x = 0; x < map.width; ++x) {
		for(unsigned y = 0; y < map.height; ++y) {
			map.cell_properties(x, y).visible = false;
		}
	}
	for(int x = monster.pos.x - monster.sight; x <= monster.pos.x + monster.sight; ++x) {
		for(int y = monster.pos.y - monster.sight; y <= monster.pos.y + monster.sight; ++y) {
			if(!map.valid(x, y)) {
				continue;
			}
			int dx = std::abs(x - monster.pos.x);
			int dy = std::abs(y - monster.pos.y);
			int distance = int(std::sqrt(dx * dx + dy * dy));
			bool can_see = distance <= monster.sight;
			if(can_see) {
				int deltax = x - monster.pos.x;
				int deltay = y - monster.pos.y;
				double error = 0.0;
				int iy = deltay > 0 ? 1 : -1;
				int ix = deltax > 0 ? 1 : -1;
				if(dx > dy) {
					double delta_error = std::abs(double(deltay) / double(deltax));
					int cy = monster.pos.y;
					for(int cx = monster.pos.x; cx != x; cx += ix) {
						if(!transparent(cx, cy)) {
							can_see = false;
							break;
						}

						error += delta_error;
						if(error > 0.5) {
							cy += iy;
							error -= 1.0;
						}
					}
				} else {
					double delta_error = std::abs(double(deltax) / double(deltay));
					int cx = monster.pos.x;
					for(int cy = monster.pos.y; cy != y; cy += iy) {
						if(!transparent(cx, cy)) {
							can_see = false;
							break;
						}

						error += delta_error;
						if(error > 0.5) {
							cx += ix;
							error -= 1.0;
						}
					}
				}
			}
			map.cell_properties(x, y).visible = can_see;
			if(can_see && monster.ai == AI::PLAYER) {
				map.cell_properties(x, y).seen_sprite = get_sprite_at(x, y);
			}
		}
	}
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
	if(!someone.godmode) {
		someone.hp -= received_damage;
	}
	game_assert(someone.is_dead(), format("{0} loses {1} hp.", someone.name, received_damage));
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
	game_assert(other.is_dead(), format("{0} hit {1} for {2} hp.", someone.name, other.name, received_damage));
	message(format("{0} hit {1} for {2} hp and kills it.", someone.name, other.name, received_damage));
	die(other);
}

void Game::move(Monster & someone, const Point & shift)
{
	game_assert(shift);
	Point new_pos = someone.pos + shift;
	game_assert(map.cell(new_pos).passable, format("{0} bump into the {1}.", someone.name, map.cell(new_pos).name));
    Door & door = find_at(doors, new_pos);
	game_assert(!door || door.opened, format("{0} is closed.", door.name));
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
		game_assert(!door.opened, format("{0} is already opened.", door.name));
		door.opened = true;
		message(format("{0} opened the {1}.", someone.name, door.name));
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
    game_assert(door.opened, format("{0} is already closed.", door.name));
    door.opened = false;
    message(format("{0} closed the {1}.", someone.name, door.name));
}

void Game::swing(Monster & someone, const Point & shift)
{
	game_assert(shift);
    Point new_pos = someone.pos + shift;
	game_assert(map.cell(new_pos).passable, format("{0} hit {1}.", someone.name, map.cell(new_pos).name));
    Door & door = find_at(doors, new_pos);
    if(door && !door.opened) {
		message(format("{0} swing at {1}.", someone.name, door.name));
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
			message(format("{0} hit {1}.", item.name, map.cell(item.pos + shift).name));
			items.push_back(item);
			break;
		}
		Door & door = find_at(doors, item.pos + shift);
		if(door && !door.opened) {
			message(format("{0} hit {1}.", item.name, door.name));
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
	message(format("{0} dropped {1} on the {2}.", someone.name, item.name, map.cell(someone.pos).name));
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
	message(format("{0} picked up {1} from the {2}.", someone.name, item.name, map.cell(someone.pos).name));
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

void Game::go_up(Monster & someone)
{
    Stairs & stair = find_at(stairs, someone.pos);
	game_assert(stair && stair.up_destination, format("{0} cannot go up from there.", someone.name));
	if(stair.up_destination < 0) {
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
	generate(stair.up_destination);
}

void Game::go_down(Monster & someone)
{
    Stairs & stair = find_at(stairs, someone.pos);
	game_assert(stair && stair.down_destination, format("{0} cannot go down from there.", someone.name));
	if(stair.down_destination < 0) {
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
	generate(stair.down_destination);
}


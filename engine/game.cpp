#include "game.h"
#include "actions.h"
#include "monsters.h"
#include <map>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <cmath>

Game::Game(LevelGenerator * level_generator)
	: current_level(0), generator(level_generator), state(PLAYING), turns(0)
{
	if(generator) {
		generator->create_types(*this);
	}
}

void Game::run(ControllerFactory controller_factory)
{
	state = PLAYING;
	while(state == PLAYING) {
		foreach(Monster & monster, level.monsters) {
			if(monster.is_dead()) {
				continue;
			}
			Controller controller = controller_factory(monster.type->ai);
			if(!controller) {
				log(format("No controller found for AI #{0}!", monster.type->ai));
				continue;
			}
			invalidate_fov(monster);
			Action * action = controller(monster, *this);
			if(action) {
				action->commit(monster, *this);
				delete action;
				action = 0;
			}
			if(state == TURN_ENDED) {
				break;
			}

			process_environment(monster);

			if(state != PLAYING) {
				break;
			}
		}
		erase_dead_monsters();
		++turns;
		if(state == TURN_ENDED) {
			state = PLAYING;
		}
	}
}

void Game::generate(int level_index)
{
	if(current_level != 0) {
		saved_levels[current_level] = level;
	}

	Monster player = get_player();
	if(saved_levels.count(level_index) > 0) {
		level = saved_levels[level_index];
		saved_levels.erase(level_index);
	} else if(generator) {
		generator->generate(level, level_index);
	} else {
		return;
	}
	if(player.valid()) {
		player.pos = get_player().pos;
		get_player() = player;
	}
	current_level = level_index;
	state = TURN_ENDED;
}

void Game::process_environment(Monster & someone)
{
	if(cell_type_at(someone.pos).hurts) {
		messages.terrain_hurts();
		hurt(someone, 1);
	}
	Object & object = find_at(level.objects, someone.pos);
	if(object.valid() && object.type->triggerable) {
		if(object.items.empty()) {
			messages.trap_is_dead(object);
		} else {
			messages.triggers_trap(someone, object);
			object.items.back().pos = object.pos;
			level.items.push_back(object.items.back());
			object.items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		messages.poisoned(someone);
		--someone.poisoning;
		hurt(someone, 1, true);
	} else if(someone.poisoning < 0) {
		someone.poisoning = 0;
	}
}

void Game::die(Monster & someone)
{
	Item item;
	while((item = someone.inventory.take_first_item()).valid()) {
		item.pos = someone.pos;
		level.items.push_back(item);
		messages.drops(someone, item);
	}
	if(someone.type->faction == Monster::PLAYER) {
		messages.player_died();
		state = PLAYER_DIED;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.inventory.worn_item().type->defence);
	someone.hp -= received_damage;
	messages.hurts(someone, received_damage);
	if(someone.is_dead()) {
		die(someone);
	}
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.inventory.worn_item().type->defence;
	other.hp -= received_damage;
	if(someone.type->poisonous) {
		messages.poisons(someone, other);
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	messages.hits(someone, other, received_damage);
	if(other.is_dead()) {
		die(other);
	}
}


const CellType & Game::cell_type_at(const Point & pos) const
{
	return *(level.map.cell(pos).type);
}


const Monster & Game::get_player() const
{
	foreach(const Monster & monster, level.monsters) {
		if(monster.type->faction == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	return empty;
}

Monster & Game::get_player()
{
	foreach( Monster & monster, level.monsters) {
		if(monster.type->faction == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	empty = Monster();
	return empty;
}

bool Game::is_passable(int x, int y) const
{
	Point new_pos(x, y);
	if(!cell_type_at(new_pos).passable) {
		return false;
	}
    const Object & object = find_at(level.objects, new_pos);
	if(object.valid() && !object.type->passable) {
		return false;
	}
    const Monster & monster = find_at(level.monsters, new_pos);
	if(monster.valid()) {
		return false;
	}
	return true;
}

bool Game::is_transparent(int x, int y) const
{
	Point new_pos(x, y);
    const Object & object = find_at(level.objects, new_pos);
	if(object.valid() && !object.type->transparent) {
		return false;
	}
	return cell_type_at(new_pos).transparent;
}

int Game::get_sprite_at(int x, int y) const
{
	return get_sprite_at(Point(x, y));
}

int Game::get_sprite_at(const Point & pos) const
{
	foreach(const Monster & monster, level.monsters) {
		if(monster.pos == pos) {
			return monster.type->sprite;
		}
	}
	foreach(const Item & item, level.items) {
		if(item.pos == pos) {
			return item.type->sprite;
		}
	}
	foreach(const Object & object, level.objects) {
		if(object.pos == pos) {
			return object.type->sprite;
		}
	}
	return cell_type_at(pos).sprite;
}

std::string Game::name_at(const Point & pos) const
{
	foreach(const Monster & monster, level.monsters) {
		if(monster.pos == pos) {
			return monster.type->name;
		}
	}
	foreach(const Item & item, level.items) {
		if(item.pos == pos) {
			return item.type->name;
		}
	}
	foreach(const Object & object, level.objects) {
		if(object.pos == pos) {
			return object.type->name;
		}
	}
	return cell_type_at(pos).name;
}

void Game::invalidate_fov(Monster & monster)
{
	for(unsigned x = 0; x < level.map.width; ++x) {
		for(unsigned y = 0; y < level.map.height; ++y) {
			level.map.cell(x, y).visible = false;
		}
	}
	for(int x = monster.pos.x - monster.type->sight; x <= monster.pos.x + monster.type->sight; ++x) {
		for(int y = monster.pos.y - monster.type->sight; y <= monster.pos.y + monster.type->sight; ++y) {
			if(!level.map.valid(x, y)) {
				continue;
			}
			int dx = std::abs(x - monster.pos.x);
			int dy = std::abs(y - monster.pos.y);
			int distance = int(std::sqrt(dx * dx + dy * dy));
			bool can_see = distance <= monster.type->sight;
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
						if(!is_transparent(cx, cy)) {
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
						if(!is_transparent(cx, cy)) {
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
			level.map.cell(x, y).visible = can_see;
			if(can_see && monster.type->faction == Monster::PLAYER) {
				level.map.cell(x, y).seen_sprite = get_sprite_at(x, y);
			}
		}
	}
}

std::list<Point> Game::find_path(const Point & player_pos, const Point & target)
{
	std::list<Point> best_path;
	if(!target.valid() || !is_passable(target.x, target.y) || player_pos == target) {
		return best_path;
	}

	std::vector<Point> shifts;
	for(Point shift(-1, 0); shift.x <= 1; ++shift.x) {
		for(shift.y = -1; shift.y <= 1; ++shift.y) {
			if(!shift.null()) {
				shifts.push_back(shift);
			}
		}
	}

	std::list<std::vector<Point> > waves;
	waves.push_front(std::vector<Point>(1, target));

	for(int i = 0; i < 2000; ++i) {
		bool found = false;
		std::vector<Point> neighs;
		foreach(const Point & point, waves.front()) {
			foreach(const Point & shift, shifts) {
				Point n = point + shift;
				if(n == player_pos) {
					found = true;
					break;
				}
				if(!is_passable(n.x, n.y)) {
					continue;
				}
				bool already_present = false;
				foreach(const std::vector<Point> & wave, waves) {
					if(std::find(wave.begin(), wave.end(), n) != wave.end()) {
						already_present = true;
						break;
					}
				}
				if(!already_present) {
					if(std::find(neighs.begin(), neighs.end(), n) == neighs.end()) {
						neighs.push_back(n);
					}
				}
			}
			if(found) {
				break;
			}
		}
		if(found) {
			break;
		}
		waves.push_front(neighs);
	}

	Point prev = player_pos;
	foreach(const std::vector<Point> wave, waves) {
		foreach(const Point & point, wave) {
			Point shift = Point(point.x - prev.x, point.y - prev.y);
			bool is_close = std::abs(shift.x) <= 1 && std::abs(shift.y) <= 1;
			if(!shift.null() && is_close) {
				prev = point;
				best_path.push_back(shift);
				break;
			}
		}
	}
	return best_path;
}

void Game::erase_dead_monsters()
{
	level.monsters.erase(std::remove_if(level.monsters.begin(), level.monsters.end(), std::mem_fun_ref(&Monster::is_dead)), level.monsters.end());
}


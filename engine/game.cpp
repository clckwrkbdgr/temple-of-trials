#include "game.h"
#include "actions.h"
#include "monsters.h"
#include "format.h"
#include <map>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <cmath>

GameEvent::GameEvent(const Info & event_actor, EventType event_type, const Info & event_target, const Info & event_help)
	: type(event_type), amount(0), actor(event_actor), target(event_target), help(event_help)
{
}

GameEvent::GameEvent(const Info & event_actor, EventType event_type, int event_amount, const Info & event_target, const Info & event_help)
	: type(event_type), amount(event_amount), actor(event_actor), target(event_target), help(event_help)
{
}


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
				try {
					action->commit(monster, *this);
				} catch(const Action::Exception & e) {
					messages.message(e);
				}
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

void Game::event(const GameEvent & e)
{
	events.push_back(e);
}

void Game::event(const Info & event_actor, GameEvent::EventType event_type, const Info & event_target, const Info & event_help)
{
	events.push_back(GameEvent(event_actor, event_type, event_target, event_help));
}

void Game::event(const Info & event_actor, GameEvent::EventType event_type, int event_amount, const Info & event_target, const Info & event_help)
{
	events.push_back(GameEvent(event_actor, event_type, event_amount, event_target, event_help));
}

void Game::events_to_messages()
{
	foreach(const GameEvent & e, events) {
		messages.message(e);
	}
	events.clear();
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
		event(cell_type_at(someone.pos), GameEvent::HURTS, someone);
		hurt(someone, 1);
	}
	Object & object = find_at(level.objects, someone.pos);
	if(object.valid() && object.type->triggerable) {
		if(object.items.empty()) {
			event(object, GameEvent::TRAP_IS_OUT_OF_ITEMS);
		} else {
			event(someone, GameEvent::TRIGGERS, object);
			object.items.back().pos = object.pos;
			level.items.push_back(object.items.back());
			object.items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		event(someone, GameEvent::IS_HURT_BY_POISONING);
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
		event(someone, GameEvent::DROPS_AT, item, cell_type_at(someone.pos));
	}
	event(someone, GameEvent::DIED);
	if(someone.type->faction == Monster::PLAYER) {
		state = PLAYER_DIED;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.inventory.worn_item().type->defence);
	someone.hp -= received_damage;
	event(someone, GameEvent::LOSES_HEALTH, received_damage);
	if(someone.is_dead()) {
		die(someone);
	}
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.inventory.worn_item().type->defence;
	other.hp -= received_damage;
	if(someone.type->poisonous) {
		event(someone, GameEvent::POISONS, other);
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	event(someone, GameEvent::HITS_FOR_HEALTH, received_damage, other);
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

CompiledInfo Game::get_info(int x, int y) const
{
	CompiledInfo result(Point(x, y));
	return result.in(level.monsters).in(level.items).in(level.objects).in(level.map);
}

CompiledInfo Game::get_info(const Point & pos) const
{
	return get_info(pos.x, pos.y);
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
						if(!get_info(cx, cy).compiled().transparent) {
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
						if(!get_info(cx, cy).compiled().transparent) {
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
				level.map.cell(x, y).seen_sprite = get_info(x, y).compiled().sprite;
			}
		}
	}
}

std::list<Point> Game::find_path(const Point & player_pos, const Point & target)
{
	std::list<Point> best_path;
	if(!get_info(target.x, target.y).compiled().passable || player_pos == target) {
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
				if(!get_info(n.x, n.y).compiled().passable) {
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


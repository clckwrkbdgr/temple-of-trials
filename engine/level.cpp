#include "level.h"
#include "game.h"
#include "objects.h"
#include <cmath>

Level::Level()
	: map(1, 1)
{
}

const Monster & Level::get_player() const
{
	foreach(const Monster & monster, monsters) {
		if(monster.faction == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	return empty;
}

Monster & Level::get_player()
{
	foreach( Monster & monster, monsters) {
		if(monster.ai == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	empty = Monster();
	return empty;
}

bool Level::is_passable(int x, int y) const
{
	Point new_pos(x, y);
	if(!map.cell(new_pos).passable) {
		return false;
	}
    const Door & door = find_at(doors, new_pos);
	if(door && !door.opened) {
		return false;
	}
    const Container & container = find_at(containers, new_pos);
	if(container) {
		return false;
	}
    const Fountain & fountain = find_at(fountains, new_pos);
	if(fountain) {
		return false;
	}
    const Monster & monster = find_at(monsters, new_pos);
	if(monster) {
		return false;
	}
	return true;
}

bool Level::is_transparent(int x, int y) const
{
	foreach(const Door & door, doors) {
		if(door.pos == Point(x, y) && !door.opened) {
			return false;
		}
	}
	return map.cell(x, y).transparent;
}

int Level::get_sprite_at(int x, int y) const
{
	return get_sprite_at(Point(x, y));
}

int Level::get_sprite_at(const Point & pos) const
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
	foreach(const Trap & trap, traps) {
		if(trap.pos == pos) {
			return trap.sprite;
		}
	}
	return map.cell(pos).sprite;
}

std::string Level::name_at(const Point & pos) const
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
	foreach(const Trap & trap, traps) {
		if(trap.pos == pos) {
			return trap.name;
		}
	}
	return map.cell(pos).name;
}

void Level::invalidate_fov(Monster & monster)
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
			map.cell_properties(x, y).visible = can_see;
			if(can_see && monster.faction == Monster::PLAYER) {
				map.cell_properties(x, y).seen_sprite = get_sprite_at(x, y);
			}
		}
	}
}

std::list<Control> Level::find_path(const Point & player_pos, const Point & target)
{
	std::list<Control> best_path;
	if(!is_passable(target.x, target.y)) {
		return best_path;
	}

	std::vector<Point> shifts;
	for(Point shift(-1, 0); shift.x <= 1; ++shift.x) {
		for(shift.y = -1; shift.y <= 1; ++shift.y) {
			if(shift) {
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
			if(shift && is_close) {
				prev = point;
				best_path.push_back(Control(Control::MOVE, shift));
				break;
			}
		}
	}
	return best_path;
}

void Level::erase_dead_monsters()
{
	monsters.erase(std::remove_if(monsters.begin(), monsters.end(), std::mem_fun_ref(&Monster::is_dead)), monsters.end());
}

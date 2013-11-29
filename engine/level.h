#pragma once
#include "map.h"
#include <vector>
#include <list>
class Monster;
class Door;
class Item;
class Container;
class Fountain;
class Stairs;
class Trap;
class Control;

struct Level {
	Map map;
	std::vector<Monster> monsters;
	std::vector<Door> doors;
	std::vector<Item> items;
	std::vector<Container> containers;
	std::vector<Fountain> fountains;
	std::vector<Stairs> stairs;
	std::vector<Trap> traps;

	Level();
	bool is_passable(int x, int y) const;
	bool is_transparent(int x, int y) const;
	int get_sprite_at(int x, int y) const;
	int get_sprite_at(const Point & pos) const;
	std::string name_at(const Point & pos) const;

	const Monster & get_player() const;
	Monster & get_player();

	std::list<Control> find_path(const Point & player_pos, const Point & target);
	void invalidate_fov(Monster & monster);
	void erase_dead_monsters();
};

class LevelGenerator {
public:
	virtual ~LevelGenerator() {}
	virtual void generate(Level & level, int level_index) = 0;
};


#pragma once
#include "map.h"
#include <vector>
#include <list>
class Monster;
class Door;
class Item;
class Object;
class Stairs;
class Trap;
class Control;

struct Level {
	Map map;
	std::vector<Monster> monsters;
	std::vector<Door> doors;
	std::vector<Item> items;
	std::vector<Object> objects;
	std::vector<Trap> traps;

	Level();
	Level(int map_width, int map_height);
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
protected:
	void fill_room(Map & map, const std::pair<Point, Point> & room, int type);
	Point random_pos(const Level & level, const std::pair<Point, Point> & room);
	std::pair<Point, Point> connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, int type);
	std::vector<std::pair<Point, Point> > shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms);
	void pop_player_front(std::vector<Monster> & monsters);
};


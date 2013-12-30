#pragma once
#include "map.h"
#include <vector>
#include <list>
class Monster;
class Item;
class Object;
class Action;
class Game;

struct Level {
	Map map;
	std::vector<Monster> monsters;
	std::vector<Item> items;
	std::vector<Object> objects;

	Level();
	~Level();
	Level(int map_width, int map_height);
};

class Dungeon {
public:
	int current_level;
	Level level;
	std::map<int, Level> saved_levels;

	Dungeon();
	virtual ~Dungeon() {}
	virtual void generate(Level & level, int level_index) = 0;
	virtual void create_types(Game & game) = 0;
protected:
	void fill_room(Map & map, const std::pair<Point, Point> & room, const CellType * type);
	std::vector<Point> random_positions(const std::pair<Point, Point> & room, int count);
	std::pair<Point, Point> connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, const CellType * type);
	std::vector<std::pair<Point, Point> > shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms);
	void pop_player_front(std::vector<Monster> & monsters);
};


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
	Level(int map_width, int map_height);
};

class LevelGenerator {
public:
	virtual ~LevelGenerator() {}
	virtual void generate(Level & level, int level_index) = 0;
protected:
	void fill_room(Map & map, const std::pair<Point, Point> & room, int type);
	Point random_pos(const Game & game, const std::pair<Point, Point> & room);
	std::pair<Point, Point> connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, int type);
	std::vector<std::pair<Point, Point> > shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms);
	void pop_player_front(std::vector<Monster> & monsters);
};


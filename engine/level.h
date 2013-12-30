#pragma once
#include "map.h"
#include "monsters.h"
#include "objects.h"
#include "items.h"
#include "cell.h"
#include <vector>
#include <list>
class Monster;
class Item;
class Object;
class Action;
class Game;
class CompiledInfo;

struct Level {
	Map map;
	std::vector<Monster> monsters;
	std::vector<Item> items;
	std::vector<Object> objects;

	Level();
	~Level();
	Level(int map_width, int map_height);

	const CellType & cell_type_at(const Point & pos) const;
	CompiledInfo get_info(int x, int y) const;
	CompiledInfo get_info(const Point & pos) const;
	const Monster & get_player() const;
	Monster & get_player();
	std::list<Point> find_path(const Point & player_pos, const Point & target);
	void invalidate_fov(Monster & monster);
	void erase_dead_monsters();
};

class Dungeon {
public:
	TypeRegistry<Cell> cell_types;
	TypeRegistry<Monster> monster_types;
	TypeRegistry<Object> object_types;
	TypeRegistry<Item> item_types;

	int current_level_index;
	std::map<int, Level> levels;

	Dungeon();
	virtual ~Dungeon() {}
	virtual void generate(Level & level, int level_index) = 0;

	Item::Builder add_item(Level & level, const std::string & type_id);
	Item::Builder add_item(Level & level, const std::string & full_type_id, const std::string & empty_type_id);
	Object::Builder add_object(Level & level, const std::string & type_id);
	Object::Builder add_object(Level & level, const std::string & closed_type_id, const std::string & opened_type_id);
	Monster::Builder add_monster(Level & level, const std::string & type_id);

	Level & level();
	const Level & level() const;
	void go_to_level(int level);
protected:
	static void fill_room(Map & map, const std::pair<Point, Point> & room, const CellType * type);
	static std::vector<Point> random_positions(const std::pair<Point, Point> & room, int count);
	static std::pair<Point, Point> connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, const CellType * type);
	static std::vector<std::pair<Point, Point> > shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms);
	static void pop_player_front(std::vector<Monster> & monsters);
};


#pragma once
#include "map.h"
#include "objects.h"
#include "level.h"
#include "message.h"
#include <map>
#include <list>
class Game;

typedef Action* (*Controller)(Monster&, Game&);
typedef Controller (*ControllerFactory)(int ai);

struct Game {
	// TODO all level stuff compile in one Dungeon class.
	int current_level;
	LevelGenerator * generator;
	Level level;
	std::map<int, Level> saved_levels;
	enum State { PLAYING, TURN_ENDED, SUSPENDED, PLAYER_DIED, COMPLETED };
	State state;
	Messages messages;
	int turns;

	std::vector<CellType> cell_types;
	const CellType & cell_type(const Cell & cell) const;
	const CellType & cell_type(const Point & pos) const;
	int add_cell_type(const CellType & cell);

	Game(LevelGenerator * level_generator = 0);
	void run(ControllerFactory controller_factory);

	void generate(int level);

	void process_environment(Monster & someone);
	void die(Monster & someone);
	void hurt(Monster & someone, int damage, bool pierce_armour = false);
	void hit(Monster & someone, Monster & other, int damage);

	bool is_passable(int x, int y) const;
	bool is_transparent(int x, int y) const;
	int get_sprite_at(int x, int y) const;
	int get_sprite_at(const Point & pos) const;
	std::string name_at(const Point & pos) const;

	const Monster & get_player() const;
	Monster & get_player();

	std::list<Point> find_path(const Point & player_pos, const Point & target);
	void invalidate_fov(Monster & monster);
	void erase_dead_monsters();
};

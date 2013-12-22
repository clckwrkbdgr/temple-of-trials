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
	int current_level;
	LevelGenerator * generator;
	Level level;
	std::map<int, Level> saved_levels;
	enum State { PLAYING, TURN_ENDED, SUSPENDED, PLAYER_DIED, COMPLETED };
	State state;
	Messages messages;
	int turns;

	Game(LevelGenerator * level_generator = 0);
	void run(ControllerFactory controller_factory);

	void generate(int level);

	Point find_random_free_cell() const;

	void process_environment(Monster & someone);
	void die(Monster & someone);
	void hurt(Monster & someone, int damage, bool pierce_armour = false);
	void hit(Monster & someone, Monster & other, int damage);
};

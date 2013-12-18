#pragma once
#include "map.h"
#include "objects.h"
#include "level.h"
#include <map>
#include <list>
class Game;

struct Control {
	enum {
		NONE,
		OPEN, CLOSE, MOVE, SMART_MOVE, SWING, WAIT,
		GRAB, DROP, WIELD, UNWIELD, WEAR, TAKE_OFF, FIRE,
		DRINK, EAT,
		GO_UP, GO_DOWN,
		COUNT
	};
	int control;
	Point direction;
	int slot;
	Control(int control = NONE);
	Control(int control, const Point & direction);
	Control(int control, int slot);
	bool done() const { return control != NONE; }
	bool operator==(const Control & other) const { return control == other.control && direction == other.direction && slot == other.slot; }
	bool operator!=(const Control & other) const { return !operator==(other); }
};

typedef Control (*Controller)(Monster&, Game&);
typedef Controller (*ControllerFactory)(int ai);

struct Game {
	bool log_messages;

	int current_level;
	LevelGenerator * generator;
	Level level;
	std::map<int, Level> saved_levels;
	enum State { PLAYING, TURN_ENDED, SUSPENDED, PLAYER_DIED, COMPLETED };
	State state;
	std::vector<std::string> messages;
	int turns;

	Game(LevelGenerator * level_generator = 0);
	void run(ControllerFactory controller_factory);

	void generate(int level);

	void message(std::string text);
	Point find_random_free_cell() const;

	void process_environment(Monster & someone);
	void die(Monster & someone);
	void hurt(Monster & someone, int damage, bool pierce_armour = false);
	void hit(Monster & someone, Monster & other, int damage);
};


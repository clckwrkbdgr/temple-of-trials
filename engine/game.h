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
};

typedef Control (*Controller)(Monster&, Game&);
typedef Controller (*ControllerFactory)(int ai);

struct Game {
	struct Message {
		std::string text;
		Message(const std::string & message_text) : text(message_text) {}
		Message() {}
	};

	bool log_messages;

	int current_level;
	LevelGenerator * generator;
	Level level;
	std::map<int, Level> saved_levels;
	bool done, player_died, completed, turn_ended;
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

	void smart_move(Monster & someone, const Point & shift);
	void move(Monster & someone, const Point & shift);
	void open(Monster & someone, const Point & shift);
	void close(Monster & someone, const Point & shift);
	void swing(Monster & someone, const Point & shift);
	void fire(Monster & someone, const Point & shift);
	void drink(Monster & someone, const Point & shift);
	void grab(Monster & someone);
	void drop(Monster & someone, int slot);
	void wield(Monster & someone, int slot);
	void unwield(Monster & someone);
	void wear(Monster & someone, int slot);
	void take_off(Monster & someone);
	void eat(Monster & someone, int slot);
	void go_up(Monster & someone);
	void go_down(Monster & someone);
};


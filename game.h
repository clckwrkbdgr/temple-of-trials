#pragma once
#include "map.h"
#include "objects.h"
#include <map>
#include <list>

struct Control {
	enum { NONE, OPEN, CLOSE, MOVE, SWING, WAIT, GRAB, DROP, WIELD, UNWIELD, WEAR, TAKE_OFF };
	int control;
	Point direction;
	int slot;
	Control(int control = NONE);
	Control(int control, const Point & direction);
	Control(int control, int slot);
	bool done() const { return control != NONE; }
};

struct Game {
	Map map;
	std::vector<Monster> monsters;
	std::vector<Door> doors;
	std::vector<Item> items;
	std::vector<Container> containers;
	bool done, player_died;
	std::list<std::string> messages;
	int turns;

	Game();
	bool load(const std::string & filename);
	bool save(const std::string & filename) const;
	int get_message_count() const;
	const std::string & get_top_message() const;

	void message(std::string text);
	Point find_random_free_cell() const;

	const Monster & getPlayer() const;

	void move(Monster & someone, const Point & shift);
	void open(Monster & someone, const Point & shift);
	void close(Monster & someone, const Point & shift);
	void swing(Monster & someone, const Point & shift);
	void grab(Monster & someone);
	void drop(Monster & someone, int slot);
	void wield(Monster & someone, int slot);
	void unwield(Monster & someone);
	void wear(Monster & someone, int slot);
	void take_off(Monster & someone);
};


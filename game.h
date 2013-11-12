#pragma once
#include "map.h"
#include "objects.h"
#include <map>
#include <list>

struct Game {
	enum { EXIT_MODE, NORMAL_MODE, OPEN_MODE, CLOSE_MODE };

	Map map;
	Monster player;
	std::vector<Monster> monsters;
	std::vector<Door> doors;
	int mode;
	std::list<std::string> messages;
	int turns;

	Game();
	void generate();
	bool load(const std::string & filename);
	bool save(const std::string & filename) const;
	void process(int ch);
	int get_message_count() const;
	const std::string & get_top_message() const;
private:
	bool turn_is_ended;
	static std::map<int, Point> directions;

	void message(const std::string & text);
	void process_normal_mode(int ch);
	void process_open_mode(int ch);
	void process_close_mode(int ch);
    Door & door_at(const Point & pos);
    Monster & monster_at(const Point & pos);
};


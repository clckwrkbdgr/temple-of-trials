#pragma once
#include <string>
class Game;
class Monster;

struct Console {
	void draw_game(const Game & game);
	int draw_and_get_control(Game & game);
	int see_messages(Game & game);
	int get_inventory_slot(const Game & game, const Monster & monster);
	void notification(const std::string & text);

	void print_tile(int x, int y, int sprite);
	void print_message(const std::string & text);
	void print_stat(int row, const std::string & text);
	void clear();
	int get_control();

	static Console & instance();
private:
	std::string notification_text;
	Console();
	~Console();
};

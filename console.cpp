#include "console.h"
#include "game.h"
#include <ncurses.h>

enum {
	MAP_WIDTH = 60,
	MAP_HEIGHT = 23
};

Console::Console()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
}

Console::~Console()
{
	cbreak();
	echo();
	curs_set(1);
	endwin();
}

void Console::print_tile(int x, int y, int sprite)
{
	mvaddch(y, x, sprite);
}

void Console::print_message(const std::string & text)
{
	mvprintw(MAP_HEIGHT, 0, "%s", text.c_str());
}

void Console::print_stat(int row, const std::string & text)
{
	mvprintw(row, MAP_WIDTH, "%s", text.c_str());
}

void Console::clear()
{
	::erase();
}

int Console::get_control()
{
	return getch();
}

Console & Console::instance()
{
	static Console console;
	return console;
}

void Console::draw_game(const Game & game)
{
	clear();
	for(unsigned x = 0; x < game.map.get_width(); ++x) {
		for(unsigned y = 0; y < game.map.get_height(); ++y) {
			print_tile(x, y, game.map.cell(x, y).sprite);
		}
	}
	for(unsigned i = 0; i < game.items.size(); ++i) {
		print_tile(game.items[i].pos.x, game.items[i].pos.y, game.items[i].sprite);
	}
	for(unsigned i = 0; i < game.doors.size(); ++i) {
		print_tile(game.doors[i].pos.x, game.doors[i].pos.y, game.doors[i].sprite());
	}
	for(unsigned i = 0; i < game.monsters.size(); ++i) {
		print_tile(game.monsters[i].pos.x, game.monsters[i].pos.y, game.monsters[i].sprite);
	}

	std::string message;
	if(game.messages.size() > 1) {
		message += format("({0}) ", game.messages.size());
	}
	if(!game.messages.empty()) {
		message += game.messages.front();
	}
	print_message(message);

	print_stat(0, format("Turns: {0}", game.turns));
	print_stat(1, format("HP   : {0}", game.getPlayer().hp));
	print_stat(2, format("Items: {0}", game.getPlayer().inventory.size()));
}

int Console::draw_and_get_control(Game & game)
{
	int ch = see_messages(game);
	return ch;
}

int Console::see_messages(Game & game)
{
	draw_game(game);
	int ch = (game.done && game.messages.empty()) ? 0 : get_control();
	while(!game.messages.empty()) {
		if(game.messages.size() > 1) {
			switch(ch) {
				case ' ': game.messages.pop_front(); break;
				default: break;
			}
			draw_game(game);
			ch = get_control();
			continue;
		} else if(game.messages.size() == 1) {
			game.messages.pop_front();
		}
		break;
	}
	return ch;
}

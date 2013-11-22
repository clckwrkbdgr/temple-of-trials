#include "console.h"
#include "game.h"
#include <ncurses.h>

enum {
	MAP_WIDTH = 60,
	MAP_HEIGHT = 1 + 23
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
	mvaddch(y + 1, x, sprite);
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

void Console::notification(const std::string & text)
{
	notification_text = text;
}

void Console::draw_game(const Game & game)
{
	clear();
	for(unsigned x = 0; x < game.map.get_width(); ++x) {
		for(unsigned y = 0; y < game.map.get_height(); ++y) {
			print_tile(x, y, game.map.cell(x, y).sprite);
		}
	}
	foreach(const Item & item, game.items) {
		print_tile(item.pos.x, item.pos.y, item.sprite);
	}
	foreach(const Container & container, game.containers) {
		print_tile(container.pos.x, container.pos.y, container.sprite);
	}
	foreach(const Door & door, game.doors) {
		print_tile(door.pos.x, door.pos.y, door.sprite());
	}
	foreach(const Monster & monster, game.monsters) {
		print_tile(monster.pos.x, monster.pos.y, monster.sprite);
	}

	std::string message;
	if(game.messages.size() > 1) {
		message += format("({0}) ", game.messages.size());
	}
	if(!game.messages.empty()) {
		message += game.messages.front();
	}
	print_message(message);

	mvprintw(0, 0, "%s", notification_text.c_str());
	notification_text.clear();

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

int Console::get_inventory_slot(const Game & game, const Monster & monster)
{
	clear();
	int width, height;
	getmaxyx(stdscr, height, width);
	int pos = 0;
	char letter = 'a';
	foreach(const Item & item, monster.inventory) {
		if(item) {
			int x = (pos < 13) ? 0 : width / 2;
			int y = 1 + ((pos < 13) ? pos : pos - 13);
			mvprintw(y, x, format("{0} - {1}", letter, item.name).c_str());
			++pos;
		}
		++letter;
		if(letter > 'z') {
			break;
		}
	}

	int slot = -1;
	std::string error;
	while(true) {
		mvprintw(0, 0, "%s", std::string(width, ' ').c_str());
		mvprintw(0, 0, "%s", error.c_str());

		int ch = getch();
		if(ch == 27) {
			nodelay(stdscr, TRUE);
			ch = getch();
			nodelay(stdscr, FALSE);
			if(ch == ERR || ch == 27) {
				slot = -1;
				break;
			}
			error = "This is not a slot";
			continue;
		}
		if(ch < 'a' || 'z' < ch) {
			error = "This is not a slot";
			continue;
		}
		slot = ch - 'a';
		if(slot >= int(monster.inventory.size()) || !monster.inventory[slot]) {
			error = "Slot is empty; nothing is here.";
			continue;
		}
		break;
	}
	return slot;
}

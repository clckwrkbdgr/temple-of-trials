#include "console.h"
#include <ncurses.h>

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

void Console::print_sprite(int x, int y, int sprite)
{
	mvaddch(y, x, sprite);
}

void Console::clear()
{
	::erase();
}

int Console::get_control()
{
	return getch();
}

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

void Console::print_player(int x, int y)
{
	mvaddch(y, x, '@');
}

void Console::clear()
{
	::erase();
}

int Console::get_control()
{
	return getch();
}

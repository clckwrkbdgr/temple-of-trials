#include <iostream>
#include <fstream>
#include <ncurses.h>
#include <time.h>

std::string now()
{
	time_t t = time(0);
	std::string s = ctime(&t);
	return s.replace(s.find('\n'), 1, " ");
}

void log(const std::string & message)
{
	static std::ofstream lout("temple.log", std::ios::app);
	lout << message << std::endl;
}

int main()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);

	log("");
	log(now());
	log("Start log.");

	int x = 0, y = 0;
	while(true) {
		mvaddch(y, x, '@');

		int ch = getch();
		if(ch == 'q') {
			break;
		}

		mvaddch(y, x, ' ');

		switch(ch) {
			case 'h': x--; break;
			case 'j': y++; break;
			case 'k': y--; break;
			case 'l': x++; break;
			case 'y': y--; x--; break;
			case 'u': y--; x++; break;
			case 'b': y++; x--; break;
			case 'n': y++; x++; break;
			default: break;
		}
	}

	cbreak();
	echo();
	curs_set(1);
	endwin();
	log("Exit.");
	return 0;
}

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

	mvprintw(0, 0, "Howdy");
	getch();

	cbreak();
	echo();
	curs_set(1);
	endwin();
	log("Exit.");
	return 0;
}

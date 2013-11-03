#include "console.h"
#include <iostream>
#include <fstream>
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
	log("Log started: " + now());
	Console console;

	int x = 0, y = 0;
	while(true) {
		console.print_player(x, y);

		int ch = console.get_control();
		if(ch == 'q') {
			break;
		}
		console.clear();

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

	log("Exiting.");
	return 0;
}

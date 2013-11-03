#include "console.h"
#include "map.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

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
	srand(time(0));
	log("Log started: " + now());
	Console console;

	Map map(60, 24, ' ');
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = '#';
	}
	int player_x = 0, player_y = 0;
	while(true) {
		for(int x = 0; x < map.get_width(); ++x) {
			for(int y = 0; y < map.get_height(); ++y) {
				console.print_sprite(x, y, map.cell(x, y));
			}
		}
		console.print_player(player_x, player_y);

		int ch = console.get_control();
		if(ch == 'q') {
			break;
		}
		console.clear();

		switch(ch) {
			case 'h': player_x--; break;
			case 'j': player_y++; break;
			case 'k': player_y--; break;
			case 'l': player_x++; break;
			case 'y': player_y--; player_x--; break;
			case 'u': player_y--; player_x++; break;
			case 'b': player_y++; player_x--; break;
			case 'n': player_y++; player_x++; break;
			default: break;
		}
	}

	log("Exiting.");
	return 0;
}

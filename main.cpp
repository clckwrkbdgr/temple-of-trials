#include "console.h"
#include "map.h"
#include "util.h"
#include <cstdlib>

int main()
{
	srand(time(0));
	log("Log started: " + now());
	Console console;

	Map map(60, 24, ' ');
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = '#';
	}
	Point player;
	while(true) {
		for(int x = 0; x < map.get_width(); ++x) {
			for(int y = 0; y < map.get_height(); ++y) {
				console.print_sprite(x, y, map.cell(x, y));
			}
		}
		console.print_player(player.x, player.y);

		int ch = console.get_control();
		if(ch == 'q') {
			break;
		}
		console.clear();

		switch(ch) {
			case 'h': player.x--; break;
			case 'j': player.y++; break;
			case 'k': player.y--; break;
			case 'l': player.x++; break;
			case 'y': player.y--; player.x--; break;
			case 'u': player.y--; player.x++; break;
			case 'b': player.y++; player.x--; break;
			case 'n': player.y++; player.x++; break;
			default: break;
		}
	}

	log("Exiting.");
	return 0;
}

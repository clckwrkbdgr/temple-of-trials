#include "console.h"
#include "map.h"
#include "objects.h"
#include "util.h"
#include <cstdlib>

int main()
{
	srand(time(0));
	log("Log started: " + now());
	Console console;

	Map map(60, 23, Cell::floor());
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = Cell::wall();
	}
	Player player = Player(Point());
	std::vector<Door> doors;
	for(int i = 0; i < 5; ++i) {
		doors.push_back(Door(Point(rand() % map.get_width(), rand() % map.get_height())));
	}

	while(true) {
		for(unsigned x = 0; x < map.get_width(); ++x) {
			for(unsigned y = 0; y < map.get_height(); ++y) {
				console.print_sprite(x, y, map.cell(x, y).sprite);
			}
		}
		for(unsigned i = 0; i < doors.size(); ++i) {
			console.print_sprite(doors[i].pos.x, doors[i].pos.y, doors[i].sprite());
		}
		console.print_sprite(player.pos.x, player.pos.y, player.sprite());

		int ch = console.get_control();
		if(ch == 'q') {
			break;
		}
		console.clear();

		Point new_pos = player.pos;
		switch(ch) {
			case 'h': new_pos.x--; break;
			case 'j': new_pos.y++; break;
			case 'k': new_pos.y--; break;
			case 'l': new_pos.x++; break;
			case 'y': new_pos.y--; new_pos.x--; break;
			case 'u': new_pos.y--; new_pos.x++; break;
			case 'b': new_pos.y++; new_pos.x--; break;
			case 'n': new_pos.y++; new_pos.x++; break;
			default: break;
		}
		bool can_move = true;
		if(!map.is_passable(new_pos)) {
			can_move = false;
		}
		for(unsigned i = 0; i < doors.size(); ++i) {
			if(doors[i].pos == new_pos) {
				can_move = false;
			}
		}

		if(can_move) {
			player.pos = new_pos;
		}
	}

	log("Exiting.");
	return 0;
}

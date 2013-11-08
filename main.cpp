#include "console.h"
#include "map.h"
#include "objects.h"
#include "util.h"
#include <map>
#include <cstdlib>

int main()
{
	srand(time(0));
	log("Log started: " + now());
	Console console;

	std::map<int, Point> directions;
	directions['h'] = Point(-1,  0);
	directions['j'] = Point( 0, +1);
	directions['k'] = Point( 0, -1);
	directions['l'] = Point(+1,  0);
	directions['y'] = Point(-1, -1);
	directions['u'] = Point(+1, -1);
	directions['b'] = Point(-1, +1);
	directions['n'] = Point(+1, +1);

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

		Point shift;
		if(directions.count(ch) > 0) {
			shift = directions[ch];
		}

		if(shift) {
			Point new_pos = player.pos + shift;
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
	}

	log("Exiting.");
	return 0;
}

#include "console.h"
#include "map.h"
#include "util.h"
#include <cstdlib>

struct Player {
	Point pos;
	Sprite sprite;
	Player(const Point & player_pos, const Sprite & player_sprite)
		: pos(player_pos), sprite(player_sprite)
	{}
};

int main()
{
	srand(time(0));
	log("Log started: " + now());
	Console console;

	Map map(60, 23, Cell(' '));
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = Cell('#', false);
	}
	Player player(Point(), '@');

	while(true) {
		for(int x = 0; x < map.get_width(); ++x) {
			for(int y = 0; y < map.get_height(); ++y) {
				console.print_sprite(x, y, map.cell(x, y).sprite);
			}
		}
		console.print_sprite(player.pos.x, player.pos.y, player.sprite);

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
		if(map.is_passable(new_pos)) {
			player.pos = new_pos;
		}
	}

	log("Exiting.");
	return 0;
}

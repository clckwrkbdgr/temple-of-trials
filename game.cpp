#include "game.h"
#include <map>
#include <cstdlib>

Game::Game()
	: map(60, 23, Cell::floor()), player(Point()), mode(NORMAL_MODE)
{
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = Cell::wall();
	}
	for(int i = 0; i < 5; ++i) {
		doors.push_back(Door(Point(rand() % map.get_width(), rand() % map.get_height())));
	}
}

std::map<int, Point> make_direction_map()
{
	std::map<int, Point> directions;
	directions['h'] = Point(-1,  0);
	directions['j'] = Point( 0, +1);
	directions['k'] = Point( 0, -1);
	directions['l'] = Point(+1,  0);
	directions['y'] = Point(-1, -1);
	directions['u'] = Point(+1, -1);
	directions['b'] = Point(-1, +1);
	directions['n'] = Point(+1, +1);
	return directions;
}

bool Game::process(int ch)
{
	static std::map<int, Point> directions = make_direction_map();

	if(ch == 'q') {
		return false;
	}

	Point shift;
	if(directions.count(ch) > 0) {
		shift = directions[ch];
	}
	Point new_pos = player.pos + shift;

	if(mode == Game::OPEN_MODE) {
		for(unsigned i = 0; i < doors.size(); ++i) {
			if(doors[i].pos == new_pos) {
				doors[i].opened = true;
			}
		}
		mode = Game::NORMAL_MODE;
	} else if(mode == Game::CLOSE_MODE) {
		for(unsigned i = 0; i < doors.size(); ++i) {
			if(doors[i].pos == new_pos) {
				doors[i].opened = false;
			}
		}
		mode = Game::NORMAL_MODE;
	} else {
		switch(ch) {
			case 'o': mode = Game::OPEN_MODE; break;
			case 'c': mode = Game::CLOSE_MODE; break;
			default: break;
		}
		if(shift) {
			bool can_move = true;
			if(!map.is_passable(new_pos)) {
				can_move = false;
			}
			for(unsigned i = 0; i < doors.size(); ++i) {
				if(doors[i].pos == new_pos && !doors[i].opened) {
					can_move = false;
				}
			}
			if(can_move) {
				player.pos = new_pos;
			}
		}
	}
	return true;
}


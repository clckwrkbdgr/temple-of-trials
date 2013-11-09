#include "game.h"
#include <map>
#include <cstdlib>

std::map<int, Point> Game::directions;

Game::Game()
	: map(60, 23, Cell::floor()), player(Point()), mode(NORMAL_MODE)
{
	if(directions.empty()) {
		directions['h'] = Point(-1,  0);
		directions['j'] = Point( 0, +1);
		directions['k'] = Point( 0, -1);
		directions['l'] = Point(+1,  0);
		directions['y'] = Point(-1, -1);
		directions['u'] = Point(+1, -1);
		directions['b'] = Point(-1, +1);
		directions['n'] = Point(+1, +1);
	}

	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = Cell::wall();
	}
	for(int i = 0; i < 5; ++i) {
		doors.push_back(Door(Point(rand() % map.get_width(), rand() % map.get_height())));
	}
}

bool Game::process(int ch)
{
	message.clear();
	switch(mode) {
		case OPEN_MODE: return process_open_mode(ch);
		case CLOSE_MODE: return process_close_mode(ch);
		case NORMAL_MODE: return process_normal_mode(ch);
		default: log("Unknown game mode!"); break;
	}
	return false;
}

bool Game::process_normal_mode(int ch)
{
	if(ch == 'q') {
		return false;
	}
	switch(ch) {
		case 'o': mode = Game::OPEN_MODE; return true;
		case 'c': mode = Game::CLOSE_MODE; return true;
		default: break;
	}
	if(directions.count(ch) > 0) {
		Point new_pos = player.pos + directions[ch];
		if(!map.is_passable(new_pos)) {
			message = "You bump into the wall.";
			return true;
		}
		for(unsigned i = 0; i < doors.size(); ++i) {
			if(doors[i].pos == new_pos && !doors[i].opened) {
				message = "Door is closed.";
				return true;
			}
		}
		player.pos = new_pos;
		return true;
	}
	message = format("Unknown control '{0}'", char(ch));
	return true;
}

bool Game::process_open_mode(int ch)
{
	if(directions.count(ch) > 0) {
		Point new_pos = player.pos + directions[ch];
		bool ok = false;
		for(unsigned i = 0; i < doors.size(); ++i) {
			if(doors[i].pos == new_pos) {
				doors[i].opened = true;
				ok = true;
				message = "You opened the door.";
			}
		}
		if(!ok) {
			message = "There is no door in that direction";
		}
	} else {
		message = "This is not a direction.";
	}
	mode = Game::NORMAL_MODE;
	return true;
}

bool Game::process_close_mode(int ch)
{
	if(directions.count(ch) > 0) {
		Point new_pos = player.pos + directions[ch];
		bool ok = false;
		for(unsigned i = 0; i < doors.size(); ++i) {
			if(doors[i].pos == new_pos) {
				doors[i].opened = false;
				ok = true;
				message = "You closed the door.";
			}
		}
		if(!ok) {
			message = "There is no door in that direction";
		}
	} else {
		message = "This is not a direction.";
	}
	mode = Game::NORMAL_MODE;
	return true;
}


#include "game.h"
#include <map>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

enum { SAVEFILE_VERSION = 1 };

std::map<int, Point> Game::directions;

Game::Game()
	: map(1, 1, Cell::floor()), player(Point()), mode(NORMAL_MODE)
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

}

void Game::generate()
{
	log("Generating new game...");
	map = Map(60, 23, Cell::floor());
	player = Player(Point());
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = Cell::wall();
	}
	for(int i = 0; i < 5; ++i) {
		doors.push_back(Door(Point(rand() % map.get_width(), rand() % map.get_height())));
	}
	log("Done.");
}

bool file_exists(const std::string & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}

#define CHECK(in) do { if(!(in).good()) { log("Error: savefile is corrupted."); return false; } } while(0)
bool Game::load(const std::string & filename)
{
	if(!file_exists(filename)) {
		return false;
	}
	std::ifstream in(filename.c_str(), std::ios::in);
	if(!in) {
		log(format("Cannot open file '{0}' for reading!", filename));
		return false;
	}

	int version;
	in >> version;
	CHECK(in);
	if(version != SAVEFILE_VERSION) {
		log(format("Savefile is of version {0}, which is incompatible with current program version {1}.", version, int(SAVEFILE_VERSION)));
		return false;
	}
	unsigned width, height;
	in >> width >> height;
	CHECK(in);
	map = Map(width, height, Cell::floor());
	for(unsigned y = 0; y < map.get_height(); ++y) {
		for(unsigned x = 0; x < map.get_width(); ++x) {
			int sprite, passable;
			in >> sprite >> passable;
			map.cell(x, y).sprite = sprite;
			map.cell(x, y).passable = passable;
			CHECK(in);
		}
	}

	in >> player.pos.x >> player.pos.y;
	CHECK(in);

	unsigned doors_count;
	in >> doors_count;
	CHECK(in);
	doors.resize(doors_count);
	for(unsigned i = 0; i < doors_count; ++i) {
		in >> doors[i].pos.x >> doors[i].pos.y >> doors[i].opened;
		CHECK(in);
	}
	return true;
}
#undef CHECK

bool Game::save(const std::string & filename) const
{
	std::ofstream out(filename.c_str(), std::ios::out);
	if(!out) {
		log(format("Cannot open file '{0}' for writing!", filename));
		return false;
	}

	out << SAVEFILE_VERSION << '\n';
	out << map.get_width() << ' ' << map.get_height() << '\n';
	for(unsigned y = 0; y < map.get_height(); ++y) {
		for(unsigned x = 0; x < map.get_width(); ++x) {
			out << int(map.cell(x, y).sprite) << ' ' << int(map.cell(x, y).passable) << ' ';
		}
		out << '\n';
	}
	out << '\n';

	out << player.pos.x << ' ' << player.pos.y << '\n';
	out << '\n';

	out << doors.size() << '\n';
	for(unsigned i = 0; i < doors.size(); ++i) {
		out << doors[i].pos.x << ' ' << doors[i].pos.y << ' ' << int(doors[i].opened) << '\n';
	}
	out << '\n';

	return true;
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
				ok = true;
				if(doors[i].opened) {
					message = "Door is already opened.";
				} else {
					doors[i].opened = true;
					message = "You opened the door.";
				}
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
				ok = true;
				if(!doors[i].opened) {
					message = "Door is already closed.";
				} else {
					doors[i].opened = false;
					message = "You closed the door.";
				}
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


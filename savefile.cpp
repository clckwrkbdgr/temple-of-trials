#include "game.h"
#include <fstream>
#include <sys/stat.h>

enum { SAVEFILE_VERSION = 4 };

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
	if(version > SAVEFILE_VERSION) {
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

	int player_sprite;
	if(SAVEFILE_VERSION <= 3) {
		in >> player.pos.x >> player.pos.y >> player_sprite;
	} else {
		in >> player.pos.x >> player.pos.y >> player_sprite >> player.ai;
	}
	CHECK(in);
	player.sprite = player_sprite;

	unsigned monsters_count;
	in >> monsters_count;
	CHECK(in);
	monsters.resize(monsters_count);
	for(unsigned i = 0; i < monsters_count; ++i) {
		int monster_sprite;
		if(SAVEFILE_VERSION <= 3) {
			in >> monsters[i].pos.x >> monsters[i].pos.y >> monster_sprite;
		} else {
			in >> monsters[i].pos.x >> monsters[i].pos.y >> monster_sprite >> monsters[i].ai;
		}
		CHECK(in);
		monsters[i].sprite = monster_sprite;
	}

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

	out << player.pos.x << ' ' << player.pos.y << ' ' << int(player.sprite) << ' ' << player.ai << '\n';
	out << '\n';

	out << monsters.size() << '\n';
	for(unsigned i = 0; i < monsters.size(); ++i) {
		out << monsters[i].pos.x << ' ' << monsters[i].pos.y << ' ' << int(monsters[i].sprite) << ' ' << monsters[i].ai << '\n';
	}
	out << '\n';

	out << doors.size() << '\n';
	for(unsigned i = 0; i < doors.size(); ++i) {
		out << doors[i].pos.x << ' ' << doors[i].pos.y << ' ' << int(doors[i].opened) << '\n';
	}
	out << '\n';

	return true;
}


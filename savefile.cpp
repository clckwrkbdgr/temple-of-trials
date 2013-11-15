#include "game.h"
#include <fstream>
#include <sys/stat.h>

enum { SAVEFILE_VERSION = 7 };

bool file_exists(const std::string & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}

std::string read_string(std::istream & in, char quote = '"')
{
	std::string result;
	char c;
	in >> std::ws;
	in.get(c);
	if(c != quote) {
		return result;
	}
	in.get(c);
	while(in.good() && c != quote) {
		if(c == '\\') {
			in.get(c);
		}
		result += c;
		in.get(c);
	}
	return result;
}

std::string escaped(const std::string & s)
{
	std::string result = s;
	size_t pos = result.find('"');
	while(pos != std::string::npos) {
		result.replace(pos, 1, "\\\"");
		pos = result.find('"');
	}
	return '"' + result + '"';
}

#define CHECK(in, text) do { if(!(in).good()) { log("Error: savefile is corrupted (reading " + to_string(text) + ") ."); return false; } } while(0)
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
	CHECK(in, "version");
	if(version > SAVEFILE_VERSION) {
		log(format("Savefile is of version {0}, which is incompatible with current program version {1}.", version, int(SAVEFILE_VERSION)));
		return false;
	}
	unsigned width, height;
	in >> width >> height;
	CHECK(in, "map size");
	map = Map(width, height, Cell::floor());
	for(unsigned y = 0; y < map.get_height(); ++y) {
		for(unsigned x = 0; x < map.get_width(); ++x) {
			int sprite, passable;
			in >> sprite >> passable;
			map.cell(x, y).sprite = sprite;
			map.cell(x, y).passable = passable;
			CHECK(in, "map cell");
		}
	}

	int player_sprite, player_pos_x, player_pos_y, player_ai;
	if(version <= 3) {
		in >> player_pos_x >> player_pos_y >> player_sprite;
	} else if(version <= 4) {
		in >> player_pos_x >> player_pos_y >> player_sprite >> player_ai;
	} else if(version <= 5) {
		in >> player_pos_x >> player_pos_y >> player_sprite >> player_ai;
		read_string(in);
	}
	CHECK(in, "player");

	unsigned monsters_count;
	in >> monsters_count;
	CHECK(in, "monster count");
	monsters.resize(monsters_count);
	for(unsigned i = 0; i < monsters_count; ++i) {
		int monster_sprite;
		if(version <= 3) {
			in >> monsters[i].pos.x >> monsters[i].pos.y >> monster_sprite;
		} else if(version <= 4) {
			int ai;
			in >> monsters[i].pos.x >> monsters[i].pos.y >> monster_sprite >> ai;
			monsters[i].ai = AI(ai == 0 ? AI::PLAYER : AI::MONSTER, ai == 1 ? AI::STILL : AI::WANDER);
		} else if(version <= 6) {
			int ai;
			in >> monsters[i].pos.x >> monsters[i].pos.y >> monster_sprite >> ai;
			monsters[i].ai = AI(ai == 0 ? AI::PLAYER : AI::MONSTER, ai == 1 ? AI::STILL : AI::WANDER);
		} else {
			in >> monsters[i].pos.x >> monsters[i].pos.y >> monster_sprite;
			in >> monsters[i].ai.faction >> monsters[i].ai.movement;
			monsters[i].name = read_string(in);
		}
		CHECK(in, "monster");
		monsters[i].sprite = monster_sprite;
	}

	unsigned doors_count;
	in >> doors_count;
	CHECK(in, "door count");
	doors.resize(doors_count);
	for(unsigned i = 0; i < doors_count; ++i) {
		in >> doors[i].pos.x >> doors[i].pos.y >> doors[i].opened;
		CHECK(in, "door");
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

	out << monsters.size() << '\n';
	for(unsigned i = 0; i < monsters.size(); ++i) {
		out << monsters[i].pos.x << ' ' << monsters[i].pos.y << ' ' << int(monsters[i].sprite) << ' ';
		out << monsters[i].ai.faction << ' ' << monsters[i].ai.movement << ' ';
		out << escaped(monsters[i].name) << '\n';
	}
	out << '\n';

	out << doors.size() << '\n';
	for(unsigned i = 0; i < doors.size(); ++i) {
		out << doors[i].pos.x << ' ' << doors[i].pos.y << ' ' << int(doors[i].opened) << '\n';
	}
	out << '\n';

	return true;
}


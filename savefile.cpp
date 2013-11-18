#include "game.h"
#include <fstream>
#include <sys/stat.h>

enum { SAVEFILE_VERSION = 8 };

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
	map = Map(width, height, Cell());
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
		if(version <= 7) {
			in >> doors[i].pos.x >> doors[i].pos.y >> doors[i].opened;
		} else {
			int opened_sprite, closed_sprite;
			in >> doors[i].pos.x >> doors[i].pos.y >> opened_sprite >> closed_sprite >> doors[i].opened;
			doors[i].opened_sprite = opened_sprite;
			doors[i].closed_sprite = closed_sprite;
		}
		CHECK(in, "door");
	}
	return true;
}
#undef CHECK

class Writer {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Writer(const std::string & filename);

	Writer & newline();
	Writer & store(int value);
	Writer & store(unsigned int value);
	Writer & store(size_t value);
	Writer & store(char value);
	Writer & store(bool value);
	Writer & store(const std::string & value);
private:
	std::ofstream out;
};

Writer::Writer(const std::string & filename)
	: out(filename.c_str(), std::ios::out)
{
	if(!out) {
		throw Exception(format("Cannot open file '{0}' for writing!", filename));
	}
}

Writer & Writer::newline()
{
	out << '\n';
	return *this;
}

Writer & Writer::store(int value)
{
	out << value << ' ';
	return *this;
}

Writer & Writer::store(unsigned int value)
{
	out << value << ' ';
	return *this;
}

Writer & Writer::store(size_t value)
{
	out << value << ' ';
	return *this;
}

Writer & Writer::store(char value)
{
	out << int(value) << ' ';
	return *this;
}

Writer & Writer::store(bool value)
{
	out << int(value) << ' ';
	return *this;
}

Writer & Writer::store(const std::string & value)
{
	out << escaped(value) << ' ';
	return *this;
}

bool Game::save(const std::string & filename) const
{
	try {
		Writer out(filename);
		out.store(SAVEFILE_VERSION);
		out.newline();
		out.store(map.get_width()).store(map.get_height());
		out.newline();
		for(unsigned y = 0; y < map.get_height(); ++y) {
			for(unsigned x = 0; x < map.get_width(); ++x) {
				out.store(map.cell(x, y).sprite).store(map.cell(x, y).passable);
			}
			out.newline();
		}
		out.newline();

		out.store(monsters.size());
		out.newline();
		for(unsigned i = 0; i < monsters.size(); ++i) {
			out.store(monsters[i].pos.x).store(monsters[i].pos.y).store(monsters[i].sprite);
			out.store(monsters[i].ai.faction).store(monsters[i].ai.movement);
			out.store(monsters[i].name);
			out.newline();
		}
		out.newline();

		out.store(doors.size());
		out.newline();
		for(unsigned i = 0; i < doors.size(); ++i) {
			out.store(doors[i].pos.x).store(doors[i].pos.y);
			out.store(doors[i].opened_sprite).store(doors[i].closed_sprite);
			out.store(doors[i].opened);
			out.newline();
		}
		out.newline();
	} catch(const Writer::Exception & e) {
		log(e.message);
		return false;
	}

	return true;
}


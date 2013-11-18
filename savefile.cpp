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

class Reader {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Reader(const std::string & filename);

	Reader & newline();
	Reader & version(int version);
	int version() const { return actual_version; }
	void check(const std::string & section);
	
	Reader & store(int & value);
	Reader & store(unsigned int & value);
	Reader & store(size_t & value);
	Reader & store(char & value);
	Reader & store(bool & value);
	Reader & store(std::string & value);
private:
	int actual_version;
	std::ifstream in;
};

Reader::Reader(const std::string & filename)
	: actual_version(SAVEFILE_VERSION), in(filename.c_str(), std::ios::in)
{
	if(!file_exists(filename)) {
		throw Exception(format("File '{0}' doesn't exists!", filename));
	}
	if(!in) {
		throw Exception(format("Cannot open file '{0}' for reading!", filename));
	}
}

Reader & Reader::newline()
{
	return *this;
}

Reader & Reader::version(int version)
{
	in >> actual_version;
	check("version");
	if(actual_version > version) {
		throw Exception(format(
					"Savefile is of version {0}, which is incompatible with current program version {1}.",
					actual_version, version
					));
	}
	return *this;
}

void Reader::check(const std::string & section)
{
	if(!in.good()) {
		throw Exception("Error: savefile is corrupted (reading " + to_string(section) + ") .");
	}
}

Reader & Reader::store(int & value)
{
	in >> value;
	return *this;
}

Reader & Reader::store(unsigned int & value)
{
	in >> value;
	return *this;
}

Reader & Reader::store(size_t & value)
{
	in >> value;
	return *this;
}

Reader & Reader::store(char & value)
{
	int stored_value;
	in >> stored_value;
	value = (char)stored_value;
	return *this;
}

Reader & Reader::store(bool & value)
{
	int stored_value;
	in >> stored_value;
	value = stored_value;
	return *this;
}

Reader & Reader::store(std::string & value)
{
	value = read_string(in);
	return *this;
}

bool Game::load(const std::string & filename)
{
	try {
		Reader in(filename);

		in.version(SAVEFILE_VERSION);
		unsigned width, height;
		in.store(width).store(height);
		in.check("map size");
		map = Map(width, height, Cell());
		for(unsigned y = 0; y < map.get_height(); ++y) {
			for(unsigned x = 0; x < map.get_width(); ++x) {
				in.store(map.cell(x, y).sprite).store(map.cell(x, y).passable);
				in.check("map cell");
			}
		}

		int player_sprite, player_pos_x, player_pos_y, player_ai;
		std::string player_name;
		if(in.version() <= 3) {
			in.store(player_pos_x).store(player_pos_y).store(player_sprite);
		} else if(in.version() <= 4) {
			in.store(player_pos_x).store(player_pos_y).store(player_sprite).store(player_ai);
		} else if(in.version() <= 5) {
			in.store(player_pos_x).store(player_pos_y).store(player_sprite).store(player_ai).store(player_name);
		}
		in.check("player");

		unsigned monsters_count;
		in.store(monsters_count);
		in.check("monster count");
		monsters.resize(monsters_count);
		for(unsigned i = 0; i < monsters_count; ++i) {
			if(in.version() <= 3) {
				in.store(monsters[i].pos.x).store(monsters[i].pos.y).store(monsters[i].sprite);
			} else if(in.version() <= 4) {
				int ai;
				in.store(monsters[i].pos.x).store(monsters[i].pos.y).store(monsters[i].sprite).store(ai);
				monsters[i].ai = AI(ai == 0 ? AI::PLAYER : AI::MONSTER, ai == 1 ? AI::STILL : AI::WANDER);
			} else if(in.version() <= 6) {
				int ai;
				in.store(monsters[i].pos.x).store(monsters[i].pos.y).store(monsters[i].sprite).store(ai);
				monsters[i].ai = AI(ai == 0 ? AI::PLAYER : AI::MONSTER, ai == 1 ? AI::STILL : AI::WANDER);
			} else {
				in.store(monsters[i].pos.x).store(monsters[i].pos.y).store(monsters[i].sprite);
				in.store(monsters[i].ai.faction).store(monsters[i].ai.movement);
				in.store(monsters[i].name);
			}
			in.check("monster");
		}

		unsigned doors_count;
		in.store(doors_count);
		in.check("door count");
		doors.resize(doors_count);
		for(unsigned i = 0; i < doors_count; ++i) {
			if(in.version() <= 7) {
				in.store(doors[i].pos.x).store(doors[i].pos.y).store(doors[i].opened);
			} else {
				in.store(doors[i].pos.x).store(doors[i].pos.y).store(doors[i].opened_sprite).store(doors[i].closed_sprite);
				in.store(doors[i].opened);
			}
			in.check("door");
		}
	} catch(const Reader::Exception & e) {
		log(e.message);
		return false;
	}
	return true;
}

class Writer {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Writer(const std::string & filename);

	Writer & newline();
	Writer & version(int version);
	int version() const { return actual_version; }
	void check(const std::string & section);
	
	Writer & store(int value);
	Writer & store(unsigned int value);
	Writer & store(size_t value);
	Writer & store(char value);
	Writer & store(bool value);
	Writer & store(const std::string & value);
private:
	int actual_version;
	std::ofstream out;
};

Writer::Writer(const std::string & filename)
	: actual_version(SAVEFILE_VERSION), out(filename.c_str(), std::ios::out)
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

Writer & Writer::version(int version)
{
	out << version << ' ';
	actual_version = version;
	return *this;
}

void Writer::check(const std::string & section)
{
	if(!out.good()) {
		throw Exception("Error: savefile is corrupted (writing " + to_string(section) + ") .");
	}
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
		out.version(SAVEFILE_VERSION);
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


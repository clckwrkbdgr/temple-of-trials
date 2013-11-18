#include "files.h"
#include "util.h"
#include "map.h"
#include <sys/stat.h>

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

std::string escaped(const std::string & s, char quote = '"')
{
	std::string result = s;
	size_t pos = result.find(quote);
	while(pos != std::string::npos) {
		result.replace(pos, 1, std::string("\\") + quote);
		pos = result.find(quote);
	}
	return quote + result + quote;
}

Reader::Reader(const std::string & filename)
	: actual_version(0), in(filename.c_str(), std::ios::in)
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

Reader & Reader::size_of(Map & map)
{
	unsigned width = 0, height = 0;
	store(width).store(height);
	map = Map(width, height, Cell());
	return *this;
}

Writer::Writer(const std::string & filename)
	: actual_version(0), out(filename.c_str(), std::ios::out)
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

Writer & Writer::size_of(const Map & map)
{
	store(map.get_width()).store(map.get_height());
	return *this;
}

#pragma once
#include "util.h"
#include <fstream>
#include <vector>
class Map;

bool file_exists(const std::string & filename);
std::string read_string(std::istream & in, char quote = '"');
std::string escaped(const std::string & s, char quote = '"');

#define SAVEFILE_STORE_EXT(Type, variable) \
	template<class Savefile> void store_ext(Savefile & savefile, Type & variable) { store_##variable(savefile, variable); } \
	template<class Savefile> void store_ext(Savefile & savefile, const Type & variable) { store_##variable(savefile, variable); } \
	template<class Savefile, class Type> \
	void store_##variable(Savefile & savefile, Type & variable)

class Reader {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Reader(std::istream & in_stream);

	Reader & newline();
	Reader & version(int major_version, int minor_version);
	int version() const { return actual_minor_version; }
	void check(const std::string & section);
	
	Reader & store(int & value);
	Reader & store(unsigned int & value);
	Reader & store(char & value);
	Reader & store(bool & value);
	Reader & store(std::string & value);
	Reader & store(Point & value);
	Reader & size_of(Map & map);
	template<class T>
	Reader & size_of(std::vector<T> & v)
	{
		unsigned size = 0;
		store(size);
		v.resize(size);
		return *this;
	}
	template<class T>
	void store(std::vector<T> & v, const std::string & name)
	{
		size_of(v);
		newline();
		check(name + " count");
		for(decltype(v.begin()) item = v.begin(); item != v.end(); ++item) {
			store_ext(*this, *item);
			newline();
			check(name);
		}
	}
private:
	int actual_minor_version;
	std::istream & in;
};

class Writer {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Writer(std::ostream & out_stream);

	Writer & newline();
	Writer & version(int major_version, int minor_version);
	int version() const { return actual_minor_version; }
	void check(const std::string & section);
	
	Writer & store(int value);
	Writer & store(unsigned int value);
	Writer & store(char value);
	Writer & store(bool value);
	Writer & store(const std::string & value);
	Writer & store(const Point & value);
	Writer & size_of(const Map & map);
	template<class T>
	Writer & size_of(const std::vector<T> & v)
	{
		store(unsigned(v.size()));
		return *this;
	}
	template<class T>
	void store(const std::vector<T> & v, const std::string & name)
	{
		size_of(v);
		newline();
		check(name + " count");
		for(decltype(v.begin()) item = v.begin(); item != v.end(); ++item) {
			store_ext(*this, *item);
			newline();
			check(name);
		}
	}
private:
	int actual_minor_version;
	std::ostream & out;
};


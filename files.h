#pragma once
#include <fstream>
#include <vector>
class Map;

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
	Reader & store(char & value);
	Reader & store(bool & value);
	Reader & store(std::string & value);
	Reader & size_of(Map & map);
	template<class T>
	Reader & size_of(std::vector<T> & v)
	{
		unsigned size = 0;
		store(size);
		v.resize(size);
		return *this;
	}
private:
	int actual_version;
	std::ifstream in;
};

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
	Writer & store(char value);
	Writer & store(bool value);
	Writer & store(const std::string & value);
	Writer & size_of(const Map & map);
	template<class T>
	Writer & size_of(const std::vector<T> & v)
	{
		store(v.size());
		return *this;
	}
private:
	int actual_version;
	std::ofstream out;
};


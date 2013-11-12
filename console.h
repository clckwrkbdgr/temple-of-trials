#pragma once
#include <string>

struct Console {
	Console();
	~Console();
	void print_tile(int x, int y, int sprite);
	void print_message(const std::string & text);
	void print_stat(int row, const std::string & text);
	void clear();
	int get_control();
};

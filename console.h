#pragma once
#include <string>

struct Console {
	Console();
	~Console();
	void print_sprite(int x, int y, int sprite);
	void print_line(int x, int y, const std::string & line);
	void clear();
	int get_control();
};

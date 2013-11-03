#pragma once

struct Console {
	Console();
	~Console();
	void print_player(int x, int y);
	void clear();
	int get_control();
};

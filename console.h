#pragma once

struct Console {
	Console();
	~Console();
	void print_player(int x, int y);
	void print_sprite(int x, int y, int sprite);
	void clear();
	int get_control();
};

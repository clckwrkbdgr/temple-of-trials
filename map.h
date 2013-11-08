#pragma once
#include <vector>
#include "util.h"

struct Cell {
	Sprite sprite;
	bool passable;

	explicit Cell(const Sprite & cell_sprite, bool is_passable = true);
	static Cell floor();
	static Cell wall();
};

class Map {
public:
	Map(unsigned map_width, unsigned map_height, Cell default_cell);
	Cell & cell(int x, int y);
	bool valid(const Point & pos) const;
	const Cell & cell(int x, int y) const;
	const Cell & cell(const Point & pos) const;
	unsigned get_width() const { return width; }
	unsigned get_height() const { return height; }
	bool is_passable(const Point & pos) const;
private:
	unsigned width, height;
	std::vector<Cell> cells;
};


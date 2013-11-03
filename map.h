#pragma once
#include <vector>

typedef int Cell;

class Map {
public:
	Map(int map_width, int map_height, Cell default_cell);
	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
	int get_width() const { return width; }
	int get_height() const { return height; }
private:
	int width, height;
	std::vector<Cell> cells;
};


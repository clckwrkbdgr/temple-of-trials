#pragma once
#include <vector>
#include "cell.h"
class Point;

class Map {
public:
	unsigned width, height;
	std::vector<Cell> cells;

	Map(unsigned map_width, unsigned map_height);
	~Map();
	bool valid(const Point & pos) const;
	bool valid(int x, int y) const;
	Cell & cell(const Point & pos);
	Cell & cell(int x, int y);
	const Cell & cell(const Point & pos) const;
	const Cell & cell(int x, int y) const;

	void fill(const CellType * celltype);
	void fill(const CellType ** map_of_celltypes);
	void set_cell_type(const Point & pos, const CellType * cell_type);
};


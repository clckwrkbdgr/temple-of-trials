#pragma once
#include <vector>
#include "util.h"

struct CellType {
	Sprite sprite;
	bool passable;
	explicit CellType(const Sprite & cell_sprite, bool is_passable = true);
	CellType();
};

struct Cell {
	int type;
	explicit Cell(int cell_type = 0);
};

class Map {
public:
	unsigned width, height;
	std::vector<CellType> celltypes;
	std::vector<Cell> cells;

	Map(unsigned map_width, unsigned map_height);
	bool valid(const Point & pos) const;
	const CellType & cell(int x, int y) const;
	const CellType & cell(const Point & pos) const;
	bool is_passable(const Point & pos) const;

	void fill(int celltype);
	int add_cell_type(const CellType & celltype);
	void set_cell_type(const Point & pos, int value);
	int & celltype(const Point & pos);
	int & celltype(int x, int y);
	int celltype(const Point & pos) const;
	int celltype(int x, int y) const;
};


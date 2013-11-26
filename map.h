#pragma once
#include <vector>
#include "util.h"

struct CellType {
	int sprite;
	bool passable;
	bool transparent;
	bool hurts;
	CellType();

	struct Builder;
};
struct CellType::Builder {
	CellType result;
	operator CellType() { return result; }
	Builder & sprite(const int & sprite);
	Builder & passable(bool value);
	Builder & transparent(bool value);
	Builder & hurts(bool value);
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
	bool valid(int x, int y) const;
	const CellType & cell(int x, int y) const;
	const CellType & cell(const Point & pos) const;

	void fill(int celltype);
	int add_cell_type(const CellType & celltype);
	void set_cell_type(const Point & pos, int value);
	int & celltype(const Point & pos);
	int & celltype(int x, int y);
	int celltype(const Point & pos) const;
	int celltype(int x, int y) const;
};


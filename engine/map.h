#pragma once
#include <vector>
#include "util.h"

struct CellType {
	int sprite;
	std::string name;
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
	Builder & name(const std::string & value);
	Builder & passable(bool value);
	Builder & transparent(bool value);
	Builder & hurts(bool value);
};

struct Cell {
	int type;
	bool visible;
	int seen_sprite;
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
	Cell & cell_properties(int x, int y);
	Cell & cell_properties(const Point & pos);
	const Cell & cell_properties(int x, int y) const;
	const Cell & cell_properties(const Point & pos) const;

	void fill(int celltype);
	void fill(int * map_of_celltypes);
	int add_cell_type(const CellType & celltype);
	void set_cell_type(const Point & pos, int value);
};


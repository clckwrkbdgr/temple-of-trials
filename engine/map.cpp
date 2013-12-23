#include "map.h"
#include "util.h"

CellType::CellType()
	: sprite(0), passable(false), transparent(false), hurts(false)
{
}

CellType::Builder & CellType::Builder::sprite(const int & value) { result.sprite = value; return *this; }
CellType::Builder & CellType::Builder::passable(bool value) { result.passable = value; return *this; }
CellType::Builder & CellType::Builder::hurts(bool value) { result.hurts = value; return *this; }
CellType::Builder & CellType::Builder::transparent(bool value) { result.transparent = value; return *this; }
CellType::Builder & CellType::Builder::name(const std::string & value) { result.name = value; return *this; }


Cell::Cell(int cell_type)
	: type(cell_type), visible(false), seen_sprite(0)
{
}


Map::Map(unsigned map_width, unsigned map_height)
	: width(map_width), height(map_height), cells(width * height, Cell(0))
{
}

const Cell & Map::cell(int x, int y) const
{
	static Cell empty;
	if(!valid(x, y)) {
		return empty;
	}
	return cells[x + y * width];
}

const Cell & Map::cell(const Point & pos) const
{
	return cell(pos.x, pos.y);
}

Cell & Map::cell(int x, int y)
{
	static Cell empty;
	empty = Cell();
	if(!valid(x, y)) {
		return empty;
	}
	return cells[x + y * width];
}

Cell & Map::cell(const Point & pos)
{
	return cell(pos.x, pos.y);
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < int(width) && y >= 0 && y < int(height));
}

bool Map::valid(const Point & pos) const
{
	return (pos.x >= 0 && pos.x < int(width) && pos.y >= 0 && pos.y < int(height));
}

void Map::fill(int celltype)
{
	cells = std::vector<Cell>(width * height, Cell(celltype));
}

void Map::fill(int * map_of_celltypes)
{
	cells = std::vector<Cell>(map_of_celltypes, map_of_celltypes + width * height);
}

void Map::set_cell_type(const Point & pos, int value)
{
	cell(pos).type = value;
}


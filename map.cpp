#include "map.h"
#include "util.h"

CellType::CellType()
	: sprite(' '), passable(false), transparent(false), hurts(false)
{
}

CellType::Builder & CellType::Builder::sprite(const int & value) { result.sprite = value; return *this; }
CellType::Builder & CellType::Builder::passable(bool value) { result.passable = value; return *this; }
CellType::Builder & CellType::Builder::hurts(bool value) { result.hurts = value; return *this; }
CellType::Builder & CellType::Builder::transparent(bool value) { result.transparent = value; return *this; }


Cell::Cell(int cell_type)
	: type(cell_type), visible(false), seen_sprite(0)
{
}


Map::Map(unsigned map_width, unsigned map_height)
	: width(map_width), height(map_height), cells(width * height, Cell(0))
{
	celltypes.push_back(CellType());
}

const CellType & Map::cell(int x, int y) const
{
	return celltypes[cell_properties(x, y).type];
}

const CellType & Map::cell(const Point & pos) const
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

int Map::add_cell_type(const CellType & celltype)
{
	celltypes.push_back(celltype);
	return celltypes.size() - 1;
}

void Map::set_cell_type(const Point & pos, int value)
{
	cell_properties(pos).type = value;
}

Cell & Map::cell_properties(int x, int y)
{
	static Cell empty;
	empty = Cell();
	if(!valid(x, y)) {
		return empty;
	}
	return cells[x + y * width];
}

Cell & Map::cell_properties(const Point & pos)
{
	return cell_properties(pos.x, pos.y);
}

const Cell & Map::cell_properties(int x, int y) const
{
	static Cell empty;
	if(!valid(x, y)) {
		return empty;
	}
	return cells[x + y * width];
}

const Cell & Map::cell_properties(const Point & pos) const
{
	return cell_properties(pos.x, pos.y);
}


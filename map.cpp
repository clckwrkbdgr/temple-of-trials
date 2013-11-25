#include "map.h"
#include "util.h"

CellType::CellType()
	: sprite(' '), passable(false), hurts(false)
{
}

CellType::Builder & CellType::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
CellType::Builder & CellType::Builder::passable(bool value) { result.passable = value; return *this; }
CellType::Builder & CellType::Builder::hurts(bool value) { result.hurts = value; return *this; }


Cell::Cell(int cell_type)
	: type(cell_type)
{
}


Map::Map(unsigned map_width, unsigned map_height)
	: width(map_width), height(map_height), cells(width * height, Cell(0))
{
	celltypes.push_back(CellType());
}

const CellType & Map::cell(int x, int y) const
{
	return celltypes[celltype(x, y)];
}

const CellType & Map::cell(const Point & pos) const
{
	return cell(pos.x, pos.y);
}

bool Map::valid(const Point & pos) const
{
	return (pos.x >= 0 && pos.x < int(width) && pos.y >= 0 && pos.y < int(height));
}

bool Map::is_passable(const Point & pos) const
{
	if(!valid(pos)) {
		return false;
	}
	return cell(pos).passable;
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
	celltype(pos) = value;
}

int & Map::celltype(int x, int y)
{
	return cells[x + y * width].type;
}

int & Map::celltype(const Point & pos)
{
	return celltype(pos.x, pos.y);
}

int Map::celltype(int x, int y) const
{
	return cells[x + y * width].type;
}

int Map::celltype(const Point & pos) const
{
	return celltype(pos.x, pos.y);
}


#include "map.h"
#include "util.h"

Cell::Cell(const Sprite & cell_sprite, bool is_passable)
	: sprite(cell_sprite), passable(is_passable)
{
}

Cell::Cell()
	: sprite(' '), passable(false)
{
}


Map::Map(unsigned map_width, unsigned map_height, Cell default_cell)
	: width(map_width), height(map_height), cells(width * height, default_cell)
{
}

Cell & Map::cell(int x, int y)
{
	return cells[x + y * width];
}

const Cell & Map::cell(int x, int y) const
{
	return cells[x + y * width];
}

const Cell & Map::cell(const Point & pos) const
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

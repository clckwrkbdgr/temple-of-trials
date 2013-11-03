#include "map.h"

Map::Map(int map_width, int map_height, Cell default_cell)
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


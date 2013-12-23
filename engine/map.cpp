#include "map.h"
#include "util.h"
#include <cmath>

Point::Point(int _x, int _y)
	: x(_x), y(_y), is_valid(true)
{
}

Point::Point()
	: x(0), y(0), is_valid(false)
{
}

bool Point::valid() const
{
	return is_valid;
}

bool Point::null() const
{
	return x == 0 && y == 0;
}

bool Point::operator==(const Point & other) const
{
	return x == other.x && y == other.y;
}

Point & Point::operator+=(const Point & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Point & Point::operator-=(const Point & other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Point & Point::operator*=(int factor)
{
	x *= factor;
	y *= factor;
	return *this;
}

Point & Point::operator/=(int factor)
{
	x /= factor;
	y /= factor;
	return *this;
}

Point operator+(const Point & a, const Point & b)
{
	Point result = a;
	return result += b;
}

Point operator-(const Point & a, const Point & b)
{
	Point result = a;
	return result -= b;
}

Point operator*(const Point & a, int factor)
{
	Point result = a;
	return result *= factor;
}

Point operator/(const Point & a, int factor)
{
	Point result = a;
	return result /= factor;
}

int distance(const Point & a, const Point & b)
{
	int x = a.x - b.x;
	int y = a.y - b.y;
	return int(std::sqrt(x * x + y * y));
}


Map::Map(unsigned map_width, unsigned map_height)
	: width(map_width), height(map_height), cells(width * height, Cell(0))
{
}

Map::~Map()
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


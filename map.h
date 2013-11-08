#pragma once
#include <vector>
class Point;

typedef char Sprite;
struct Cell {
	Sprite sprite;
	bool passable;
	explicit Cell(const Sprite & cell_sprite, bool is_passable = true) : sprite(cell_sprite), passable(is_passable) {}
};

class Map {
public:
	Map(int map_width, int map_height, Cell default_cell);
	Cell & cell(int x, int y);
	bool valid(const Point & pos) const;
	const Cell & cell(int x, int y) const;
	const Cell & cell(const Point & pos) const;
	int get_width() const { return width; }
	int get_height() const { return height; }
	bool is_passable(const Point & pos) const;
private:
	int width, height;
	std::vector<Cell> cells;
};


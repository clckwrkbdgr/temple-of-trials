#pragma once
#include <string>

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
	typedef CellType Type;
	unsigned type;
	bool visible;
	int seen_sprite;
	explicit Cell(int cell_type = 0);
};


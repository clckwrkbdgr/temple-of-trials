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
	const Type * type;
	std::string type_id;
	bool visible;
	int seen_sprite;
	explicit Cell(const std::string & cell_type = std::string());
};


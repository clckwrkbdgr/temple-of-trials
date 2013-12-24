#include "cell.h"

CellType::CellType()
	: sprite(0), passable(false), transparent(false), hurts(false)
{
}

CellType::Builder & CellType::Builder::sprite(const int & value) { result.sprite = value; return *this; }
CellType::Builder & CellType::Builder::passable(bool value) { result.passable = value; return *this; }
CellType::Builder & CellType::Builder::hurts(bool value) { result.hurts = value; return *this; }
CellType::Builder & CellType::Builder::transparent(bool value) { result.transparent = value; return *this; }
CellType::Builder & CellType::Builder::name(const std::string & value) { result.name = value; return *this; }


Cell::Cell(const std::string & cell_type)
	: type_id(cell_type), visible(false), seen_sprite(0)
{
}



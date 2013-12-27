#pragma once
#include <string>
class Item;
class Object;
class Monster;
class Cell;

struct Info {
	const std::string & id;
	const int & sprite;
	const std::string & name;
	const bool & passable;
	const bool & transparent;

	Info();
	Info(const Item & base);
	Info(const Object & base);
	Info(const Monster & base);
	Info(const Cell & base);
	bool valid() const;
};

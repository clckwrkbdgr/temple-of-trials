#pragma once
#include <string>
#include <vector>
#include "util.h"
class Item;
class Object;
class Monster;
class Cell;
class Point;
class Map;

struct Info {
	std::string id;
	int sprite;
	std::string name;
	bool passable;
	bool transparent;

	Info();
	Info(const Item & base);
	Info(const Object & base);
	Info(const Monster & base);
	Info(const Cell & base);
	bool valid() const;
};

struct CompiledInfo {
	const Point & pos;
	std::vector<Info> all_info;
	CompiledInfo(const Point & target_pos) : pos(target_pos) {}
	template<class T>
	CompiledInfo & in(const std::vector<T> & v)
	{
		foreach(const T & t, v) {
			if(t.pos == pos) {
				all_info.push_back(Info(t));
			}
		}
		return *this;
	}
	CompiledInfo & in(const Map & map);
	bool passable() const;
	bool transparent() const;
};

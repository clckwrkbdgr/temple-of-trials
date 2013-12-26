#include "level.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "items.h"
#include "format.h"
#include <algorithm>

Level::Level()
	: map(1, 1)
{
}

Level::~Level()
{
}

Level::Level(int map_width, int map_height)
	: map(map_width, map_height)
{
}


void LevelGenerator::fill_room(Map & map, const std::pair<Point, Point> & room, const CellType * type)
{
	for(int x = room.first.x; x <= room.second.x; ++x) {
		for(int y = room.first.y; y <= room.second.y; ++y) {
			map.set_cell_type(Point(x, y), type);
		}
	}
}

std::vector<Point> LevelGenerator::random_positions(const std::pair<Point, Point> & room, int count)
{
	std::vector<Point> result;
	for(int i = 0; i < count; ++i) {
		int width = (room.second.x - room.first.x);
		int height = (room.second.y - room.first.y);
		int counter = width * height;
		while(--counter > 0) {
			int x = rand() % width + room.first.x;
			int y = rand() % height + room.first.y;
			if(result.end() == std::find(result.begin(), result.end(), Point(x, y))) {
				result << Point(x, y);
				break;
			}
		}
	}
	for(int i = result.size(); i < count; ++i) {
		result << room.first;
	}
	return result;
}

std::pair<Point, Point> LevelGenerator::connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, const CellType * type)
{
	if(a.first.x < b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = a.second.x + 1; x != b.first.x; ++x) {
			level.map.set_cell_type(Point(x, way), type);
		}
		return std::make_pair(Point(a.second.x + 1, way), Point(b.first.x - 1, way));
	}
	if(a.first.x > b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = b.second.x + 1; x != a.first.x; ++x) {
			level.map.set_cell_type(Point(x, way), type);
		}
		return std::make_pair(Point(b.second.x + 1, way), Point(a.first.x - 1, way));
	}
	if(a.first.y < b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = a.second.y + 1; y != b.first.y; ++y) {
			level.map.set_cell_type(Point(wax, y), type);
		}
		return std::make_pair(Point(wax, a.second.y + 1), Point(wax, b.first.y - 1));
	}
	if(a.first.y > b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = b.second.y + 1; y != a.first.y; ++y) {
			level.map.set_cell_type(Point(wax, y), type);
		}
		return std::make_pair(Point(wax, b.second.y + 1), Point(wax, a.first.y - 1));
	}
	return std::make_pair(Point(), Point());
}

std::vector<std::pair<Point, Point> > LevelGenerator::shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms)
{
	static int a00[] = { 8, 1, 2, 7, 0, 3, 6, 5, 4, };
	static int a01[] = { 6, 7, 8, 5, 0, 1, 4, 3, 2, };
	static int a02[] = { 4, 5, 6, 3, 0, 7, 2, 1, 8, };
	static int a03[] = { 2, 3, 4, 1, 0, 5, 8, 7, 6, };
	static int a04[] = { 2, 1, 8, 3, 0, 7, 4, 5, 6, };
	static int a05[] = { 8, 7, 6, 1, 0, 5, 2, 3, 4, };
	static int a06[] = { 6, 5, 4, 7, 0, 3, 8, 1, 2, };
	static int a07[] = { 4, 3, 2, 5, 0, 1, 6, 7, 8, };
	static int a08[] = { 0, 1, 2, 5, 4, 3, 6, 7, 8, };
	static int a09[] = { 0, 1, 2, 7, 6, 3, 8, 5, 4, };
	static int a10[] = { 0, 1, 2, 7, 8, 3, 6, 5, 4, };
	static int a11[] = { 0, 5, 6, 1, 4, 7, 2, 3, 8, };
	static int a12[] = { 0, 7, 8, 1, 6, 5, 2, 3, 4, };
	static int a13[] = { 0, 7, 6, 1, 8, 5, 2, 3, 4, };
	static int a14[] = { 6, 7, 8, 5, 4, 3, 0, 1, 2, };
	static int a15[] = { 8, 5, 4, 7, 6, 3, 0, 1, 2, };
	static int a16[] = { 6, 5, 4, 7, 8, 3, 0, 1, 2, };
	static int a17[] = { 2, 3, 8, 1, 4, 7, 0, 5, 6, };
	static int a18[] = { 2, 3, 4, 1, 6, 5, 0, 7, 8, };
	static int a19[] = { 2, 3, 4, 1, 8, 5, 0, 7, 6, };
	static int a20[] = { 2, 1, 0, 3, 4, 5, 8, 7, 6, };
	static int a21[] = { 2, 1, 0, 3, 6, 7, 4, 5, 8, };
	static int a22[] = { 2, 1, 0, 3, 8, 7, 4, 5, 6, };
	static int a23[] = { 6, 5, 0, 7, 4, 1, 8, 3, 2, };
	static int a24[] = { 8, 7, 0, 5, 6, 1, 4, 3, 2, };
	static int a25[] = { 6, 7, 0, 5, 8, 1, 4, 3, 2, };
	static int a26[] = { 8, 7, 6, 3, 4, 5, 2, 1, 0, };
	static int a27[] = { 4, 5, 8, 3, 6, 7, 2, 1, 0, };
	static int a28[] = { 4, 5, 6, 3, 8, 7, 2, 1, 0, };
	static int a29[] = { 8, 3, 2, 7, 4, 1, 6, 5, 0, };
	static int a30[] = { 4, 3, 2, 5, 6, 1, 8, 7, 0, };
	static int a31[] = { 4, 3, 2, 5, 8, 1, 6, 7, 0, };
	static int * layouts[] = {
		a00, a01, a02, a03, a04, a05, a06, a07,
		a08, a09, a10, a11, a12, a13, a14, a15,
		a16, a17, a18, a19, a20, a21, a22, a23,
		a24, a25, a26, a27, a28, a29, a30, a31,
	};
	int * a = layouts[rand() % 32];

	std::vector<std::pair<Point, Point> > new_rooms(9);
	for(unsigned i = 0; i < rooms.size(); ++i) {
		new_rooms[a[i]] = rooms[i];
	}
	return new_rooms;
}

void LevelGenerator::pop_player_front(std::vector<Monster> & monsters)
{
	if(monsters.empty()) {
		return;
	}
	foreach(Monster & monster, monsters) {
		if(monster.type->faction == Monster::PLAYER) {
			std::swap(monster, monsters.front());
			log("Player found.");
			break;
		}
	}
}

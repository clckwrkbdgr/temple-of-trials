#include "generate.h"
#include "ai.h"
#include "sprites.h"
#include <cstdlib>

namespace World {

CellType floor()
{
	return CellType::Builder().sprite(Sprites::FLOOR).name("floor").passable(true).transparent(true);
}

CellType wall()
{
	return CellType::Builder().sprite(Sprites::WALL).name("wall").passable(false);
}

CellType torch()
{
	return CellType::Builder().sprite(Sprites::TORCH).name("torch").passable(false).transparent(true);
}

CellType goo()
{
	return CellType::Builder().sprite(Sprites::GOO).name("goo").passable(true).hurts(true).transparent(true);
}

Item explosive(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::EXPLOSIVE).name("explosive").quest();
}

Item money(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::MONEY).name("money");
}

Item scorpion_tail(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::SCORPION_TAIL).name("scorpion tail");
}

Item spear(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::SPEAR).name("spear").damage(5);
}

Item jacket(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::JACKET).name("jacket").wearable().defence(1);
}

Item antidote(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::ANTIDOTE).name("antidote").edible().antidote(5);
}

Item apple(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::APPLE).name("apple").edible().healing(10);
}

Monster player(const Point & monster_pos)
{
	return Monster::Builder().pos(monster_pos).sprite(Sprites::PLAYER).sight(10).hp(20).ai(AI::PLAYER).name("you").hit_strength(3);
}

Monster ant(int ai, const Point & monster_pos)
{
	return Monster::Builder().pos(monster_pos).sprite(Sprites::ANT).sight(6).hp(3).ai(ai).name("ant").hit_strength(1);
}

Monster scorpion(int ai, const Point & monster_pos)
{
	return Monster::Builder().pos(monster_pos).sprite(Sprites::SCORPION).sight(8).hp(5).ai(ai).name("scorpion").
		item(scorpion_tail()).hit_strength(2).poisonous(true);
}

Door door(const Point & pos)
{
	return Door::Builder().pos(pos).opened_sprite(Sprites::DOOR_OPENED).closed_sprite(Sprites::DOOR_CLOSED).name("door").opened(false);
}

Container pot(const Point & pos)
{
	return Container::Builder().pos(pos).sprite(Sprites::POT).name("pot").item(money()).item(antidote());
}

Fountain well(const Point & pos)
{
	return Fountain::Builder().pos(pos).sprite(Sprites::WELL).name("well");
}

Stairs gate(const Point & pos)
{
	return Stairs::Builder().pos(pos).sprite(Sprites::GATE).name("gate").up_destination(-1);
}

Stairs stairs_up(const Point & pos, int destination)
{
	return Stairs::Builder().pos(pos).sprite(Sprites::STAIRS_UP).name("stairs").up_destination(destination);
}

Stairs stairs_down(const Point & pos, int destination)
{
	return Stairs::Builder().pos(pos).sprite(Sprites::STAIRS_DOWN).name("stairs").down_destination(destination);
}

}

void fill_room(Map & map, const std::pair<Point, Point> & room, int type)
{
	for(int x = room.first.x; x <= room.second.x; ++x) {
		for(int y = room.first.y; y <= room.second.y; ++y) {
			map.set_cell_type(Point(x, y), type);
		}
	}
}

Point random_pos(const Level & level, const std::pair<Point, Point> & room)
{
	int width = (room.second.x - room.first.x);
	int height = (room.second.y - room.first.y);
	int counter = width * height;
	while(--counter > 0) {
		int x = rand() % width + room.first.x;
		int y = rand() % height + room.first.y;
		if(level.is_passable(x, y)) {
			return Point(x, y);
		}
	}
	return room.first;
}

void connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, int type)
{
	Point a_center = a.first + (a.second - a.first) / 2;
	Point b_center = b.first + (b.second - b.first) / 2;

	if(a.first.x < b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = a.second.x + 1; x != b.first.x; ++x) {
			level.map.set_cell_type(Point(x, way), type);
		}
		level.doors.push_back(World::door(Point(a.second.x + 1, way)));
		level.doors.push_back(World::door(Point(b.first.x - 1, way)));
		return;
	}
	if(a.first.x > b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = b.second.x + 1; x != a.first.x; ++x) {
			level.map.set_cell_type(Point(x, way), type);
		}
		level.doors.push_back(World::door(Point(b.second.x + 1, way)));
		level.doors.push_back(World::door(Point(a.first.x - 1, way)));
		return;
	}
	if(a.first.y < b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = a.second.y + 1; y != b.first.y; ++y) {
			level.map.set_cell_type(Point(wax, y), type);
		}
		level.doors.push_back(World::door(Point(wax, a.second.y + 1)));
		level.doors.push_back(World::door(Point(wax, b.first.y - 1)));
		return;
	}
	if(a.first.y > b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = b.second.y + 1; y != a.first.y; ++y) {
			level.map.set_cell_type(Point(wax, y), type);
		}
		level.doors.push_back(World::door(Point(wax, b.second.y + 1)));
		level.doors.push_back(World::door(Point(wax, a.first.y - 1)));
		return;
	}
}

std::vector<std::pair<Point, Point> > random_transmutation(const std::vector<std::pair<Point, Point> > & rooms)
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

void generate_level(Level & level, int level_index)
{
	Monster player = level.get_player();
	level.monsters.clear();
	level.doors.clear();
	level.items.clear();
	level.containers.clear();
	level.fountains.clear();
	level.stairs.clear();
	log("Game cleared.");

	level.map = Map(60, 23);

	int floor_type = level.map.add_cell_type(World::floor());
	int wall_type = level.map.add_cell_type(World::wall());
	int goo_type = level.map.add_cell_type(World::goo());
	int torch_type = level.map.add_cell_type(World::torch());

	level.map.fill(wall_type);
	log("Map filled.");

	std::vector<std::pair<Point, Point> > rooms;
	for(int y = 0; y < 3; ++y) {
		for(int x = 0; x < 3; ++x) {
			int cell_width = level.map.width / 3;
			int cell_height = level.map.height / 3;
			Point topleft = Point(x * cell_width + 2, y * cell_height + 2);
			Point bottomright = Point(x * cell_width + cell_width - 2, y * cell_height + cell_height - 2);
			rooms.push_back(std::make_pair(topleft, bottomright));
		}
	}
	rooms = random_transmutation(rooms);
	log("Rooms arranged.");

	std::vector<std::string> room_content;
	switch(level_index) {
		case 1:
			room_content
				<< "@<" << "a" << "%a"
				<< "####Aa" << "%Aa" << "&%AAa"
				<< "&&(AAA" << "&&&&%A%A" << "####AAAA>"
				;
			break;
		case 2:
			room_content
				<< "&@<%" << "AAAV" << "########S%%"
				<< "(SaAAA" << "V%%%" << "AAASS"
				<< "&&&&~~~~~~~~~~~~{%" << "V[SSSAA%" << "####SSSSAAAA>"
				;
			break;
		case 3:
			room_content
				<< "####&@<%%(" << "~~~~~~~~VSSSS" << "####AAASSS%%"
				<< "####~~~~SSSAAAAA%%" << "####~~~~VSSSSS%" << "####~~~~VVSSSSSSSAAA%%%"
				<< "####~~~~VVSSSSSSSSSS" << "####VVV%%%%%%%(" << "SSSSSSSSAAAAAAAA~~~~~~~~~~~~>"
				;
			break;
		default:
			room_content
				<< "@<" << std::string(32, '~') << std::string(32, '~')
				<< std::string(32, '~') << std::string(32, '~') << std::string(32, '~')
				<< std::string(32, '~') << std::string(32, '~') << std::string(32, '~')
				;
			break;
	}

	for(unsigned i = 0; i < rooms.size(); ++i) {
		fill_room(level.map, rooms[i], floor_type);
		foreach(char cell, room_content[i]) {
			Point pos = random_pos(level, rooms[i]);
			switch(cell) {
				case '#' : level.map.set_cell_type(pos, wall_type); break;
				case '~' : level.map.set_cell_type(pos, goo_type); break;
				case ' ' : level.map.set_cell_type(pos, floor_type); break;
				case '&' : level.map.set_cell_type(pos, torch_type); break;

				case '$' : level.items.push_back(World::money(pos)); break;
				case '%' : level.items.push_back(World::apple(pos)); break;
				case '(' : level.items.push_back(World::spear(pos)); break;
				case '*' : level.items.push_back(World::explosive(pos)); break;
				case '[' : level.items.push_back(World::jacket(pos)); break;

				case '{' : level.fountains.push_back(World::well(pos)); break;
				case '+' : level.doors.push_back(World::door(pos)); break;
				case 'V' : level.containers.push_back(World::pot(pos)); break;
				case '^' : level.stairs.push_back(World::gate(pos)); break;
				case '>' :
					if(level_index == 3) {
						level.items.push_back(World::explosive(pos));
					} else {
						level.stairs.push_back(World::stairs_down(pos, level_index + 1));
					}
					break;
				case '<' :
					if(level_index == 1) {
						level.stairs.push_back(World::gate(pos)); break;
					} else {
						level.stairs.push_back(World::stairs_up(pos, level_index - 1)); break;
					}
					break;
				case '@' :
					if(player) {
						player.pos = pos;
						level.monsters.push_back(player);
					} else {
						level.monsters.push_back(World::player(pos));
					}
					break;
				case 'a' : level.monsters.push_back(World::ant(AI::ANGRY_AND_STILL, pos)); break;
				case 'A' : level.monsters.push_back(World::ant(AI::ANGRY_AND_WANDER, pos)); break;
				case 'S' : level.monsters.push_back(World::scorpion(AI::ANGRY_AND_STILL, pos)); break;
				default: log("Unknown cell: '{0}' in room {1}.", cell, i);
			}
		}
		if(i > 0) {
			connect_rooms(level, rooms[i], rooms[i - 1], floor_type);
		}
	}
	log("Rooms filled.");
	if(!level.monsters.empty()) {
		foreach(Monster & monster, level.monsters) {
			if(monster.ai == AI::PLAYER) {
				std::swap(monster, level.monsters[0]);
				log("Player found.");
			}
		}
	}
	log("Player popped.");
}

void LinearGenerator::generate(Level & level, int level_index)
{
	log("Generating level {0}...", level_index);

	generate_level(level, level_index);

	log("Done.");
}


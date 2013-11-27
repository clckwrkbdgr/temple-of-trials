#include "generate.h"
#include "ai.h"
#include "game.h"
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
	return Stairs::Builder().pos(pos).sprite(Sprites::GATE).name("gate");
}

}

void predefined_demo_level(Game & game)
{
	std::vector<std::string> level;
	level.push_back("############################################################");
	level.push_back("#                                     #~~~~~~~~~~#         #");
	level.push_back("#        # a #%(%# a #                #~~ {  &$~~#  #&S&#  #");
	level.push_back("#                              #      #~~  [( $~~#  S * S  #");
	level.push_back("#     ###################      #      #~~~~~~~~~~#  #&S&#  #");
	level.push_back("#           #                  #      #~   ~ ~~  #         #");
	level.push_back("#  A        #              S   #  A   #~ ~ a###$ #####+#####");
	level.push_back("#      %    #     #            #      #   ~ $~ # #         #");
	level.push_back("#           #     #     ########      # ~ ~ ~    #  #   #  #");
	level.push_back("########    #     #         A     ^   # ~$~~  #  #  A &    #");
	level.push_back("#           #     #                  $# a~ # a~  #   &&&A  #");
	level.push_back("#    a      #     ##################### ~#~  #~  #    &    #");
	level.push_back("#           #                        $#  # $~    #^ #   #  #");
	level.push_back("#######+#####     ########     #      #   ~a~  # #       $$#");
	level.push_back("#   a       #          $#      #   A  # ~#   ~ ~ ###+###+###");
	level.push_back("#           #       a% $#   S  #      #  ~#~~    #         #");
	level.push_back("#           #          $#      #      #  #~~~ ~  #   A     #");
	level.push_back("####+####################      #      #####+######         #");
	level.push_back("#       #    %   #          &  #      #          #   A  S  #");
	level.push_back("#       #    ^   +             #             A   +         #");
	level.push_back("#   @   #$   (   #          S  #                 #         #");
	level.push_back("#  <<<  #$$$     #             #                 #         #");
	level.push_back("############################################################");

	game.map = Map(60, 23);

	for(unsigned y = 0; y < game.map.height; ++y) {
		for(unsigned x = 0; x < game.map.width; ++x) {
			switch(level[y][x]) {
				case '@' : game.monsters.push_back(World::player(Point(x, y))); break;
			}
		}
	}

	int floor_type = game.map.add_cell_type(World::floor());
	int wall_type = game.map.add_cell_type(World::wall());
	int goo_type = game.map.add_cell_type(World::goo());
	int torch_type = game.map.add_cell_type(World::torch());

	for(unsigned y = 0; y < game.map.height; ++y) {
		for(unsigned x = 0; x < game.map.width; ++x) {
			char cell = level[y][x];
			Point pos(x, y);
			game.map.set_cell_type(pos, floor_type);
			switch(cell) {
				case '#' : game.map.set_cell_type(pos, wall_type); break;
				case '~' : game.map.set_cell_type(pos, goo_type); break;
				case ' ' : game.map.set_cell_type(pos, floor_type); break;
				case '&' : game.map.set_cell_type(pos, torch_type); break;

				case '$' : game.items.push_back(World::money(pos)); break;
				case '%' : game.items.push_back(World::apple(pos)); break;
				case '(' : game.items.push_back(World::spear(pos)); break;
				case '*' : game.items.push_back(World::explosive(pos)); break;
				case '[' : game.items.push_back(World::jacket(pos)); break;

				case '{' : game.fountains.push_back(World::well(pos)); break;
				case '+' : game.doors.push_back(World::door(pos)); break;
				case '<' : game.stairs.push_back(World::gate(pos)); break;
				case '^' : game.containers.push_back(World::pot(pos)); break;

				case 'a' : game.monsters.push_back(World::ant(AI::ANGRY_AND_STILL, pos)); break;
				case 'A' : game.monsters.push_back(World::ant(AI::ANGRY_AND_WANDER, pos)); break;
				case 'S' : game.monsters.push_back(World::scorpion(AI::ANGRY_AND_STILL, pos)); break;
				default: log("Unknown cell in level: '{0}' at {1},{2}.", cell, x, y);
			}
		}
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

Point random_pos(const Game & game, const std::pair<Point, Point> & room)
{
	Game::MapPassabilityDetector detector(game);
	int width = (room.second.x - room.first.x);
	int height = (room.second.y - room.first.y);
	int counter = width * height;
	while(--counter > 0) {
		int x = rand() % width + room.first.x;
		int y = rand() % height + room.first.y;
		if(detector.is_passable(x, y)) {
			return Point(x, y);
		}
	}
	return room.first;
}

void connect_rooms(Map & map, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, int type)
{
	Point a_center = a.first + (a.second - a.first) / 2;
	Point b_center = b.first + (b.second - b.first) / 2;

	if(a.first.x < b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = a.second.x + 1; x != b.first.x; ++x) {
			map.set_cell_type(Point(x, way), type);
		}
		return;
	}
	if(a.first.x > b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = b.first.x + 1; x != a.second.x; ++x) {
			map.set_cell_type(Point(x, way), type);
		}
		return;
	}
	if(a.first.y < b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = a.second.y + 1; y != b.first.y; ++y) {
			map.set_cell_type(Point(wax, y), type);
		}
		return;
	}
	if(a.first.y > b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = b.first.y + 1; y != a.second.y; ++y) {
			map.set_cell_type(Point(wax, y), type);
		}
		return;
	}
}

std::vector<std::pair<Point, Point> > random_transmutation(const std::vector<std::pair<Point, Point> > & rooms)
{
	static int a[] = {
		0, 1, 2,
		5, 4, 3,
		6, 7, 8,
	};
	std::vector<std::pair<Point, Point> > new_rooms(9);
	for(unsigned i = 0; i < rooms.size(); ++i) {
		new_rooms[a[i]] = rooms[i];
	}
	return new_rooms;
}

void generate_level(Game & game)
{
	game.map = Map(60, 23);

	int floor_type = game.map.add_cell_type(World::floor());
	int wall_type = game.map.add_cell_type(World::wall());
	int goo_type = game.map.add_cell_type(World::goo());
	int torch_type = game.map.add_cell_type(World::torch());

	game.map.fill(wall_type);

	std::vector<std::pair<Point, Point> > rooms;
	for(int y = 0; y < 3; ++y) {
		for(int x = 0; x < 3; ++x) {
			int cell_width = game.map.width / 3;
			int cell_height = game.map.height / 3;
			Point topleft = Point(x * cell_width + 2, y * cell_height + 2);
			Point bottomright = Point(x * cell_width + cell_width - 2, y * cell_height + cell_height - 2);
			rooms.push_back(std::make_pair(topleft, bottomright));
		}
	}
	rooms = random_transmutation(rooms);

	std::vector<std::string> room_content;
	room_content.push_back("&@<");
	room_content.push_back("####&%a");
	room_content.push_back("##&aA%");
	room_content.push_back("##&(A");
	room_content.push_back("&&aAA{V");
	room_content.push_back("&&%SAA");
	room_content.push_back("&&%SS");
	room_content.push_back("[SSS~~~~~~~");
	room_content.push_back("##SSSS*~~~~~~~");

	for(unsigned i = 0; i < rooms.size(); ++i) {
		fill_room(game.map, rooms[i], floor_type);
		foreach(char cell, room_content[i]) {
			Point pos = random_pos(game, rooms[i]);
			switch(cell) {
				case '#' : game.map.set_cell_type(pos, wall_type); break;
				case '~' : game.map.set_cell_type(pos, goo_type); break;
				case ' ' : game.map.set_cell_type(pos, floor_type); break;
				case '&' : game.map.set_cell_type(pos, torch_type); break;

				case '$' : game.items.push_back(World::money(pos)); break;
				case '%' : game.items.push_back(World::apple(pos)); break;
				case '(' : game.items.push_back(World::spear(pos)); break;
				case '*' : game.items.push_back(World::explosive(pos)); break;
				case '[' : game.items.push_back(World::jacket(pos)); break;

				case '{' : game.fountains.push_back(World::well(pos)); break;
				case '+' : game.doors.push_back(World::door(pos)); break;
				case 'V' : game.containers.push_back(World::pot(pos)); break;
				case '<' : game.stairs.push_back(World::gate(pos)); break;

				case '@' : game.monsters.push_back(World::player(pos)); break;
				case 'a' : game.monsters.push_back(World::ant(AI::ANGRY_AND_STILL, pos)); break;
				case 'A' : game.monsters.push_back(World::ant(AI::ANGRY_AND_WANDER, pos)); break;
				case 'S' : game.monsters.push_back(World::scorpion(AI::ANGRY_AND_STILL, pos)); break;
				default: log("Unknown cell: '{0}' in room {1}.", cell, i);
			}
		}
		if(i > 0) {
			connect_rooms(game.map, rooms[i], rooms[i - 1], floor_type);
		}
	}
	if(!game.monsters.empty()) {
		foreach(Monster & monster, game.monsters) {
			if(monster.ai == AI::PLAYER) {
				std::swap(monster, game.monsters[0]);
			}
		}
	}
}

void generate(Game & game)
{
	log("Generating new game...");
	game = Game();

	generate_level(game);

	log("Done.");
}


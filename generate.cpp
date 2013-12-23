#include "generate.h"
#include "ai.h"
#include "sprites.h"
#include "engine/monsters.h"
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

Item sharpened_pole(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::SHARPENED_POLE).name("sharpened pole").damage(1);
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

Item key(int key_type, const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite(Sprites::KEY).name("key").key_type(key_type);
}

Monster player(const Point & monster_pos)
{
	return Monster::Builder().faction(Monster::PLAYER).pos(monster_pos).sprite(Sprites::PLAYER).sight(10).hp(20).ai(AI::PLAYER).name("you").hit_strength(3);
}

Monster ant(int ai, const Point & monster_pos)
{
	return Monster::Builder().faction(Monster::MONSTER).pos(monster_pos).sprite(Sprites::ANT).sight(6).hp(3).ai(ai).name("ant").hit_strength(1);
}

Monster scorpion(int ai, const Point & monster_pos)
{
	return Monster::Builder().faction(Monster::MONSTER).pos(monster_pos).sprite(Sprites::SCORPION).sight(8).hp(5).ai(ai).name("scorpion").
		item(scorpion_tail()).hit_strength(2).poisonous(true);
}

Object door(const Point & pos)
{
	return Object::Builder().pos(pos).opened_sprite(Sprites::DOOR_OPENED).closed_sprite(Sprites::DOOR_CLOSED).name("door").openable().opened(false).passable().transparent();
}

Object locked_door(const Point & pos, int lock_type)
{
	return Object::Builder().pos(pos).opened_sprite(Sprites::DOOR_OPENED).closed_sprite(Sprites::DOOR_CLOSED).name("door").openable().opened(false).passable().transparent().locked(true).lock_type(lock_type);
}

Object pot(const Point & pos)
{
	return Object::Builder().pos(pos).sprite(Sprites::POT).name("pot").containable().item(money()).item(antidote()).transparent();
}

Object well(const Point & pos)
{
	return Object::Builder().pos(pos).sprite(Sprites::WELL).name("well").drinkable().transparent();
}

Object gate(const Point & pos)
{
	return Object::Builder().pos(pos).sprite(Sprites::GATE).name("gate").transporting().up_destination(-1).passable().transparent();
}

Object stairs_up(const Point & pos, int destination)
{
	return Object::Builder().pos(pos).sprite(Sprites::STAIRS_UP).name("stairs").transporting().up_destination(destination).passable().transparent();
}

Object stairs_down(const Point & pos, int destination)
{
	return Object::Builder().pos(pos).sprite(Sprites::STAIRS_DOWN).name("stairs").transporting().down_destination(destination).passable().transparent();
}

Object trap(const Point & pos)
{
	return Object::Builder().pos(pos).sprite(Sprites::TRAP).name("trap").triggerable().item(sharpened_pole()).passable().transparent();
}

}

void LinearGenerator::create_types(Game & game)
{
	floor_type = game.add_cell_type(World::floor());
	wall_type = game.add_cell_type(World::wall());
	goo_type = game.add_cell_type(World::goo());
	torch_type = game.add_cell_type(World::torch());
}

void LinearGenerator::generate(Level & level, int level_index)
{
	log("Generating level {0}...", level_index);

	level = Level(60, 23);
	log("Level cleared.");

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
	rooms = shuffle_rooms(rooms);
	log("Rooms arranged.");

	std::vector<std::string> room_content;
	switch(level_index) {
		case 1:
			room_content
				<< "^@<" << "a" << "%a"
				<< "####^Aa" << "%Aa" << "&%AAa"
				<< "&&(AAA" << "&&&&^%A%A" << "####AAAA>"
				;
			break;
		case 2:
			room_content
				<< "&@<%" << "AAAV" << "########^^S%%"
				<< "(SaAAA" << "V%%%" << "AAASS"
				<< "&&&&~~~~~~~~~~~~{%" << "V[SSSAA%" << "####SSSSAAAA>"
				;
			break;
		case 3:
			room_content
				<< "####&^^@<%%(" << "~~~~~~~~^VSSSS" << "####AAASSS%%"
				<< "####~~~~SSSAAAAA%%" << "####~~~~VSSSSS%" << "####~~~~VVSSSSSSSAAA%%%"
				<< "####~~~~VVSSSSSSSSSS" << "####VVV^^^%%%%%%%(" << "SSSSSSSSAAAAAAAA~~~~~~~~~~~~>"
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
		bool is_last_room = i == rooms.size() - 1;
		if(is_last_room) {
			if(!level.monsters.empty()) {
				int key_holder = rand() % level.monsters.size();
				level.monsters[key_holder].inventory.insert(World::key(level_index));
			}
		}
		fill_room(level.map, rooms[i], floor_type);
		std::vector<Point> positions = random_positions(rooms[i], room_content[i].size());
		foreach(char cell, room_content[i]) {
			Point pos = positions.back();
			positions.pop_back();
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

				case '{' : level.objects.push_back(World::well(pos)); break;
				case '+' : level.objects.push_back(World::door(pos)); break;
				case 'V' : level.objects.push_back(World::pot(pos)); break;
				case '^' : level.objects.push_back(World::trap(pos)); break;
				case '>' :
					if(level_index == 3) {
						level.items.push_back(World::explosive(pos));
					} else {
						level.objects.push_back(World::stairs_down(pos, level_index + 1));
					}
					break;
				case '<' :
					if(level_index == 1) {
						level.objects.push_back(World::gate(pos)); break;
					} else {
						level.objects.push_back(World::stairs_up(pos, level_index - 1)); break;
					}
					break;
				case '@' :
					level.monsters.push_back(World::player(pos));
					break;
				case 'a' : level.monsters.push_back(World::ant(AI::ANGRY_AND_STILL, pos)); break;
				case 'A' : level.monsters.push_back(World::ant(AI::ANGRY_AND_WANDER, pos)); break;
				case 'S' : level.monsters.push_back(World::scorpion(AI::ANGRY_AND_STILL, pos)); break;
				default: log("Unknown cell: '{0}' in room {1}.", cell, i);
			}
		}
		if(i > 0) {
			std::pair<Point, Point> doors = connect_rooms(level, rooms[i], rooms[i - 1], floor_type);
			if(doors.first.valid() && doors.second.valid()) {
				level.objects.push_back(World::door(doors.first));
				if(is_last_room) {
					level.objects.push_back(World::locked_door(doors.second, level_index));
				} else {
					level.objects.push_back(World::door(doors.second));
				}
			}
		}
	}
	log("Rooms filled.");
	pop_player_front(level.monsters);
	log("Player popped.");

	log("Done.");
}


#include "generate.h"
#include "ai.h"
#include "sprites.h"
#include "engine/monsters.h"
#include "engine/format.h"
#include <cstdlib>

void LinearDungeon::create_types(Game & game)
{
	floor = game.cell_types.insert(CellType::Builder("floor").sprite(Sprites::FLOOR).name("floor").passable(true).transparent(true));
	wall = game.cell_types.insert(CellType::Builder("wall").sprite(Sprites::WALL).name("wall").passable(false));
	goo = game.cell_types.insert(CellType::Builder("goo").sprite(Sprites::GOO).name("goo").passable(true).hurts(true).transparent(true));
	torch = game.cell_types.insert(CellType::Builder("torch").sprite(Sprites::TORCH).name("torch").passable(false).transparent(true));

	player = game.monster_types.insert(MonsterType::Builder("player").faction(Monster::PLAYER).
			sprite(Sprites::PLAYER).sight(10).max_hp(20).ai(AI::PLAYER).name("you").hit_strength(3));
	wander_ant = game.monster_types.insert(MonsterType::Builder("wander_ant").faction(Monster::MONSTER).
			sprite(Sprites::ANT).sight(6).max_hp(3).ai(AI::ANGRY_AND_WANDER).name("ant").hit_strength(1));
	still_ant = game.monster_types.insert(MonsterType::Builder("still_ant").faction(Monster::MONSTER).
			sprite(Sprites::ANT).sight(6).max_hp(3).ai(AI::ANGRY_AND_STILL).name("ant").hit_strength(1));
	scorpion = game.monster_types.insert(MonsterType::Builder("scorpion").faction(Monster::MONSTER).
			sprite(Sprites::SCORPION).sight(8).max_hp(5).ai(AI::ANGRY_AND_STILL).name("scorpion").hit_strength(2).poisonous(true));

	opened_door = game.object_types.insert(ObjectType::Builder("opened_door").sprite(Sprites::DOOR_OPENED).name("door").openable().passable().transparent());
	closed_door = game.object_types.insert(ObjectType::Builder("closed_door").sprite(Sprites::DOOR_CLOSED).name("door").openable());
	pot = game.object_types.insert(ObjectType::Builder("pot").sprite(Sprites::POT).name("pot").containable().transparent());
	well = game.object_types.insert(ObjectType::Builder("well").sprite(Sprites::WELL).name("well").drinkable().transparent());
	gate = game.object_types.insert(ObjectType::Builder("gate").sprite(Sprites::GATE).name("gate").transporting().passable().transparent());
	stairs_up = game.object_types.insert(ObjectType::Builder("stairs_up").sprite(Sprites::STAIRS_UP).name("stairs").transporting().passable().transparent());
	stairs_down = game.object_types.insert(ObjectType::Builder("stairs_down").sprite(Sprites::STAIRS_DOWN).name("stairs").transporting().passable().transparent());
	trap = game.object_types.insert(ObjectType::Builder("trap").sprite(Sprites::TRAP).name("trap").triggerable().passable().transparent());

	explosive = game.item_types.insert(ItemType::Builder("explosive").sprite(Sprites::EXPLOSIVE).name("explosive").quest());
	money = game.item_types.insert(ItemType::Builder("money").sprite(Sprites::MONEY).name("money"));
	scorpion_tail = game.item_types.insert(ItemType::Builder("scorpion_tail").sprite(Sprites::SCORPION_TAIL).name("scorpion tail"));
	spear = game.item_types.insert(ItemType::Builder("spear").sprite(Sprites::SPEAR).name("spear").damage(5));
	sharpened_pole = game.item_types.insert(ItemType::Builder("sharpened_pole").sprite(Sprites::SHARPENED_POLE).name("sharpened pole").damage(1));
	jacket = game.item_types.insert(ItemType::Builder("jacket").sprite(Sprites::JACKET).name("jacket").wearable().defence(1));
	antidote = game.item_types.insert(ItemType::Builder("antidote").sprite(Sprites::ANTIDOTE).name("antidote").edible().antidote(5));
	apple = game.item_types.insert(ItemType::Builder("apple").sprite(Sprites::APPLE).name("apple").edible().healing(10));
	key = game.item_types.insert(ItemType::Builder("key").sprite(Sprites::KEY).name("key"));
	empty_flask = game.item_types.insert(ItemType::Builder("empty_flask").sprite(Sprites::FLASK).name("empty flask"));
	full_flask = game.item_types.insert(ItemType::Builder("full_flask").sprite(Sprites::FLASK).name("water flask").edible().healing(5));
}

void LinearDungeon::generate(Level & level, int level_index)
{
	log("Generating level {0}...", level_index);

	level = Level(60, 23);
	log("Level cleared.");

	level.map.fill(wall);
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
				<< "^@}<" << "a" << "%a"
				<< "####^Aa" << "%Aa" << "&%AAa}"
				<< "&&(AAA" << "&&&&^%A%A" << "####AAAA>"
				;
			break;
		case 2:
			room_content
				<< "&@<%}" << "AAAV" << "########^^S%%"
				<< "(SaAAA" << "V%%%}" << "AAASS"
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
				<< std::string(32, '~') << std::string(32, '~') << "*";
				;
			break;
	}

	for(unsigned i = 0; i < rooms.size(); ++i) {
		bool is_last_room = i == rooms.size() - 1;
		if(is_last_room) {
			if(!level.monsters.empty()) {
				int key_holder = rand() % level.monsters.size();
				level.monsters[key_holder].inventory.insert(Item::Builder(key).key_type(level_index));
			}
		}
		fill_room(level.map, rooms[i], floor);
		std::vector<Point> positions = random_positions(rooms[i], room_content[i].size());
		foreach(char cell, room_content[i]) {
			Point pos = positions.back();
			positions.pop_back();
			switch(cell) {
				case '#' : level.map.set_cell_type(pos, wall); break;
				case '~' : level.map.set_cell_type(pos, goo); break;
				case ' ' : level.map.set_cell_type(pos, floor); break;
				case '&' : level.map.set_cell_type(pos, torch); break;

				case '$' : level.items.push_back(Item::Builder(money).pos(pos)); break;
				case '%' : level.items.push_back(Item::Builder(apple).pos(pos)); break;
				case '(' : level.items.push_back(Item::Builder(spear).pos(pos)); break;
				case '*' : level.items.push_back(Item::Builder(explosive).pos(pos)); break;
				case '[' : level.items.push_back(Item::Builder(jacket).pos(pos)); break;
				case '}' : level.items.push_back(Item::Builder(full_flask, empty_flask).pos(pos)); break;

				case '{' : level.objects.push_back(Object::Builder(well).pos(pos)); break;
				case '+' : level.objects.push_back(Object::Builder(closed_door, opened_door).pos(pos).opened(false)); break;
				case 'V' : level.objects.push_back(Object::Builder(pot).pos(pos).item(antidote).item(money)); break;
				case '^' : level.objects.push_back(Object::Builder(trap).pos(pos).item(sharpened_pole)); break;
				case '>' :
					if(level_index == 3) {
						level.items.push_back(Item::Builder(explosive).pos(pos));
					} else {
						level.objects.push_back(Object::Builder(stairs_down).pos(pos).down_destination(level_index + 1));
					}
					break;
				case '<' :
					if(level_index == 1) {
						level.objects.push_back(Object::Builder(gate).pos(pos).up_destination(-1)); break;
					} else {
						level.objects.push_back(Object::Builder(stairs_up).pos(pos).up_destination(level_index - 1)); break;
					}
					break;
				case '@' :
					level.monsters.push_back(Monster::Builder(player).pos(pos));
					break;
				case 'a' : level.monsters.push_back(Monster::Builder(still_ant).pos(pos)); break;
				case 'A' : level.monsters.push_back(Monster::Builder(wander_ant).pos(pos)); break;
				case 'S' : level.monsters.push_back(Monster::Builder(scorpion).pos(pos).item(scorpion_tail)); break;
				default: log("Unknown cell: '{0}' in room {1}.", cell, i);
			}
		}
		if(i > 0) {
			std::pair<Point, Point> doors = connect_rooms(level, rooms[i], rooms[i - 1], floor);
			if(!doors.first.null() && !doors.second.null()) {
				level.objects.push_back(Object::Builder(closed_door, opened_door).pos(doors.first));
				if(is_last_room) {
					level.objects.push_back(Object::Builder(closed_door, opened_door).pos(doors.second).locked(true).lock_type(level_index));
				} else {
					level.objects.push_back(Object::Builder(closed_door, opened_door).pos(doors.second));
				}
			}
		}
	}
	log("Rooms filled.");
	pop_player_front(level.monsters);
	log("Player popped.");

	log("Done.");
}


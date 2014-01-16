#include "generate.h"
#include "sprites.h"
#include <chthon/log.h>
#include <cstdlib>
using namespace Chthon;

namespace AI {
	enum { DUMMY, PLAYER, ANGRY_AND_WANDER, ANGRY_AND_STILL, CALM_AND_STILL };
}

LinearDungeon::LinearDungeon(Controller * player_controller)
	: Game()
{
	controller_factory.add_controller(AI::PLAYER, player_controller);
	controller_factory.add_controller(AI::ANGRY_AND_WANDER,
			(new BasicAI())->add(BasicAI::MOVE_TO_HIT_PLAYER_IF_SEES)->add(BasicAI::HIT_PLAYER_IF_NEAR)->add(BasicAI::MOVE_RANDOM)
			);
	controller_factory.add_controller(AI::ANGRY_AND_STILL,
			(new BasicAI())->add(BasicAI::MOVE_TO_HIT_PLAYER_IF_SEES)->add(BasicAI::HIT_PLAYER_IF_NEAR)
			);
	controller_factory.add_controller(AI::CALM_AND_STILL,
			(new BasicAI())->add(BasicAI::HIT_PLAYER_IF_NEAR)->add(BasicAI::WAIT)
			);

	cell_types.insert("floor").sprite(Sprites::FLOOR).name("floor").passable(true).transparent(true);
	cell_types.insert("wall").sprite(Sprites::WALL).name("wall").passable(false);
	cell_types.insert("goo").sprite(Sprites::GOO).name("goo").passable(true).hurts(true).transparent(true);
	cell_types.insert("torch").sprite(Sprites::TORCH).name("torch").passable(false).transparent(true);

	monster_types.insert("player").faction(Monster::PLAYER).sprite(Sprites::PLAYER).sight(10).max_hp(20).ai(AI::PLAYER).name("you").hit_strength(3);
	monster_types.insert("wander_ant").faction(Monster::MONSTER).sprite(Sprites::ANT).
		sight(6).max_hp(3).ai(AI::ANGRY_AND_WANDER).name("ant").hit_strength(1);
	monster_types.insert("still_ant").faction(Monster::MONSTER).sprite(Sprites::ANT).
		sight(6).max_hp(3).ai(AI::ANGRY_AND_STILL).name("ant").hit_strength(1);
	monster_types.insert("scorpion").faction(Monster::MONSTER).sprite(Sprites::SCORPION).
		sight(8).max_hp(5).ai(AI::ANGRY_AND_STILL).name("scorpion").hit_strength(2).poisonous(true);

	object_types.insert("opened_door").sprite(Sprites::DOOR_OPENED).name("door").openable().passable().transparent();
	object_types.insert("closed_door").sprite(Sprites::DOOR_CLOSED).name("door").openable();
	object_types.insert("pot").sprite(Sprites::POT).name("pot").containable().transparent();
	object_types.insert("well").sprite(Sprites::WELL).name("well").drinkable().transparent();
	object_types.insert("gate").sprite(Sprites::GATE).name("gate").transporting().passable().transparent();
	object_types.insert("stairs_up").sprite(Sprites::STAIRS_UP).name("stairs").transporting().passable().transparent();
	object_types.insert("stairs_down").sprite(Sprites::STAIRS_DOWN).name("stairs").transporting().passable().transparent();
	object_types.insert("trap").sprite(Sprites::TRAP).name("trap").triggerable().passable().transparent();

	item_types.insert("explosive").sprite(Sprites::EXPLOSIVE).name("explosive").quest();
	item_types.insert("money").sprite(Sprites::MONEY).name("money");
	item_types.insert("scorpion_tail").sprite(Sprites::SCORPION_TAIL).name("scorpion tail");
	item_types.insert("spear").sprite(Sprites::SPEAR).name("spear").damage(5);
	item_types.insert("sharpened_pole").sprite(Sprites::SHARPENED_POLE).name("sharpened pole").damage(1);
	item_types.insert("jacket").sprite(Sprites::JACKET).name("jacket").wearable().defence(1);
	item_types.insert("antidote").sprite(Sprites::ANTIDOTE).name("antidote").edible().antidote(5);
	item_types.insert("apple").sprite(Sprites::APPLE).name("apple").edible().healing(10);
	item_types.insert("key").sprite(Sprites::KEY).name("key");
	item_types.insert("empty_flask").sprite(Sprites::FLASK).name("empty flask");
	item_types.insert("full_flask").sprite(Sprites::FLASK).name("water flask").edible().healing(5);
}

void LinearDungeon::generate(Level & level, int level_index)
{
	log("Generating level {0}...", level_index);

	level = Level(60, 23);
	log("Level cleared.");

	level.map.fill(Cell(cell_types.get("wall")));
	log("Map filled.");

	std::vector<std::pair<Point, Point> > rooms;
	for(int y = 0; y < 3; ++y) {
		for(int x = 0; x < 3; ++x) {
			unsigned cell_width = level.map.width / 3;
			unsigned cell_height = level.map.height / 3;
			Point topleft = Point(x * int(cell_width) + 2, y * int(cell_height) + 2);
			Point bottomright = Point(x * int(cell_width) + int(cell_width) - 2, y * int(cell_height) + int(cell_height) - 2);
			rooms.push_back(std::make_pair(topleft, bottomright));
		}
	}
	rooms = DungeonBuilder::shuffle_rooms(rooms);
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
				unsigned key_holder = (unsigned)rand() % level.monsters.size();
				level.monsters[key_holder].inventory.insert(Item::Builder(item_types.get("key")).key_type(level_index));
			}
		}
		DungeonBuilder::fill_room(level.map, rooms[i], cell_types.get("floor"));
		std::vector<Point> positions = DungeonBuilder::random_positions(rooms[i], room_content[i].size());
		foreach(char cell, room_content[i]) {
			Point pos = positions.back();
			positions.pop_back();
			switch(cell) {
				case '#' : level.map.cell(pos) = Cell(cell_types.get("wall")); break;
				case '~' : level.map.cell(pos) = Cell(cell_types.get("goo")); break;
				case ' ' : level.map.cell(pos) = Cell(cell_types.get("floor")); break;
				case '&' : level.map.cell(pos) = Cell(cell_types.get("torch")); break;

				case '$' : add_item(level, "money").pos(pos); break;
				case '%' : add_item(level, "apple").pos(pos); break;
				case '(' : add_item(level, "spear").pos(pos); break;
				case '*' : add_item(level, "explosive").pos(pos); break;
				case '[' : add_item(level, "jacket").pos(pos); break;
				case '}' : add_item(level, "full_flask", "empty_flask").pos(pos); break;

				case '{' : add_object(level, "well").pos(pos); break;
				case '+' : add_object(level, "closed_door", "opened_door").pos(pos).opened(false); break;
				case 'V' : add_object(level, "pot").pos(pos).item(item_types.get("antidote")).item(item_types.get("money")); break;
				case '^' : add_object(level, "trap").pos(pos).item(item_types.get("sharpened_pole")); break;
				case '>' :
					if(level_index == 3) {
						add_item(level, "explosive").pos(pos);
					} else {
						add_object(level, "stairs_down").pos(pos).down_destination(level_index + 1);
					}
					break;
				case '<' :
					if(level_index == 1) {
						add_object(level, "gate").pos(pos).up_destination(-1); break;
					} else {
						add_object(level, "stairs_up").pos(pos).up_destination(level_index - 1); break;
					}
					break;
				case '@' :
					add_monster(level, "player").pos(pos);
					break;
				case 'a' : add_monster(level, "still_ant").pos(pos); break;
				case 'A' : add_monster(level, "wander_ant").pos(pos); break;
				case 'S' : add_monster(level, "scorpion").pos(pos).item(item_types.get("scorpion_tail")); break;
				default: log("Unknown cell: '{0}' in room {1}.", cell, i);
			}
		}
		if(i > 0) {
			std::pair<Point, Point> doors = DungeonBuilder::connect_rooms(level, rooms[i], rooms[i - 1], cell_types.get("floor"));
			if(!doors.first.null() && !doors.second.null()) {
				add_object(level, "closed_door", "opened_door").pos(doors.first);
				if(is_last_room) {
					add_object(level, "closed_door", "opened_door").pos(doors.second).locked(true).lock_type(level_index);
				} else {
					add_object(level, "closed_door", "opened_door").pos(doors.second);
				}
			}
		}
	}
	log("Rooms filled.");
	DungeonBuilder::pop_player_front(level.monsters);
	log("Player popped.");

	log("Done.");
}


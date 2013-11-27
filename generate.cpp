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

void generate(Game & game)
{
	log("Generating new game...");
	game = Game();

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

	log("Done.");
}


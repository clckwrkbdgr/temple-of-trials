#include "generate.h"
#include "ai.h"
#include "game.h"
#include <cstdlib>

namespace World {

CellType floor()
{
	return CellType::Builder().sprite('.').passable(true);
}

CellType wall()
{
	return CellType::Builder().sprite('#').passable(false);
}

Item money(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite('$').name("money");
}

Item scorpion_tail(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite('!').name("scorpion tail");
}

Item spear(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite('(').name("spear").damage(5);
}

Item jacket(const Point & pos = Point())
{
	return Item::Builder().pos(pos).sprite('[').name("jacket").wearable().defence(1);
}

Monster player(const Point & monster_pos)
{
	return Monster::Builder().pos(monster_pos).sprite('@').sight(10).hp(20).ai(AI::PLAYER).name("you").hit_strength(3);
}

Monster ant(int ai, const Point & monster_pos)
{
	return Monster::Builder().pos(monster_pos).sprite('A').sight(6).hp(3).ai(ai).name("ant").hit_strength(1);
}

Monster scorpion(int ai, const Point & monster_pos)
{
	return Monster::Builder().pos(monster_pos).sprite('S').sight(8).hp(5).ai(ai).name("scorpion").item(scorpion_tail()).
		hit_strength(2);
}

Door door(const Point & pos)
{
	return Door::Builder().pos(pos).opened_sprite('-').closed_sprite('+').opened(false);
}

Container pot(const Point & pos)
{
	return Container::Builder().pos(pos).sprite('^').name("pot").item(money()).item(jacket());
}

}

void generate(Game & game)
{
	log("Generating new game...");
	game.doors.clear();
	game.monsters.clear();
	game.map = Map(60, 23);
	int floor_type = game.map.add_cell_type(World::floor());
	int wall_type = game.map.add_cell_type(World::wall());
	game.map.fill(floor_type);
	for(int i = 0; i < 10; ++i) {
		game.map.set_cell_type(game.find_random_free_cell(), wall_type);
	}

	game.monsters.push_back(World::player(game.find_random_free_cell()));
	for(int i = 0; i < 5; ++i) {
		game.doors.push_back(World::door(game.find_random_free_cell()));
	}
	for(int i = 0; i < 5; ++i) {
		game.items.push_back(World::money(game.find_random_free_cell()));
	}
	game.containers.push_back(World::pot(game.find_random_free_cell()));
	game.items.push_back(World::spear(game.find_random_free_cell()));
	for(int i = 0; i < 5; ++i) {
		int ai = AI::CALM_AND_STILL;
		switch(rand() % 3) {
			case 0: ai = AI::ANGRY_AND_WANDER; break;
			case 1: ai = AI::ANGRY_AND_STILL; break;
			case 2: ai = AI::CALM_AND_STILL; break;
			default: break;
		}
		game.monsters.push_back(World::ant(ai, game.find_random_free_cell()));
	}
	game.monsters.push_back(World::scorpion(AI::ANGRY_AND_WANDER, game.find_random_free_cell()));
	log("Done.");
}


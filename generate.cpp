#include "generate.h"
#include "ai.h"
#include "game.h"
#include <cstdlib>

namespace World {

Cell floor()
{
	return Cell('.');
}

Cell wall()
{
	return Cell('#', false);
}

Monster player(const Point & monster_pos)
{
	Monster::Builder player;
	return player.pos(monster_pos).sprite('@').sight(10).hp(20).ai(AI::PLAYER).name("you");
}

Monster ant(int ai, const Point & monster_pos)
{
	Monster::Builder ant;
	return ant.pos(monster_pos).sprite('A').sight(6).hp(3).ai(ai).name("ant");
}

Door door(const Point & pos)
{
	Door::Builder door;
	return door.pos(pos).opened_sprite('-').closed_sprite('+').opened(false);
}

Item money(const Point & pos)
{
	Item::Builder money;
	return money.pos(pos).sprite('$').name("money");
}

}

void generate(Game & game)
{
	log("Generating new game...");
	game.doors.clear();
	game.monsters.clear();
	game.map = Map(60, 23, World::floor());
	game.monsters.push_back(World::player(game.find_random_free_cell()));
	for(int i = 0; i < 10; ++i) {
		Point point = game.find_random_free_cell();
		if(point) {
			game.map.cell(point.x, point.y) = World::wall();
		}
	}
	for(int i = 0; i < 5; ++i) {
		Point point = game.find_random_free_cell();
		if(point) {
			game.doors.push_back(World::door(point));
		}
	}
	for(int i = 0; i < 5; ++i) {
		Point point = game.find_random_free_cell();
		if(point) {
			game.items.push_back(World::money(point));
		}
	}
	for(int i = 0; i < 5; ++i) {
		Point point = game.find_random_free_cell();
		if(point) {
			int ai = AI::CALM_AND_STILL;
			switch(rand() % 3) {
				case 0: ai = AI::ANGRY_AND_WANDER; break;
				case 1: ai = AI::ANGRY_AND_STILL; break;
				case 2: ai = AI::CALM_AND_STILL; break;
				default: break;
			}
			game.monsters.push_back(World::ant(ai, point));
		}
	}
	log("Done.");
}


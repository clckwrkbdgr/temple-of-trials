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
	Monster player(monster_pos, '@', 10, 20, AI::PLAYER);
	player.name = "you";
	return player;
}

Monster ant(int ai, const Point & monster_pos)
{
	Monster ant(monster_pos, 'A', 6, 3, ai);
	ant.name = "ant";
	return ant;
}

Door door(const Point & pos)
{
	return Door(pos, '-', '+', false);
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
			int ai = rand() % 4 + 2;
			game.monsters.push_back(World::ant(ai, point));
		}
	}
	log("Done.");
}


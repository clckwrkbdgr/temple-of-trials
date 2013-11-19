#include "ai.h"
#include "game.h"
#include "objects.h"

Control angry_and_wander(Monster & monster, Game & game)
{
	const Monster & player = game.getPlayer();
	int d = distance(monster.pos, player.pos);
	if(1 <= d && d <= monster.sight) {
		Point shift = Point(
				sign(player.pos.x - monster.pos.x),
				sign(player.pos.y - monster.pos.y)
				);
		return Control(Control::MOVE, shift);
	}
	return Control(Control::MOVE, Point(rand() % 3 - 1, rand() % 3 - 1));
}

Control angry_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.getPlayer();
	int d = distance(monster.pos, player.pos);
	if(1 <= d && d <= monster.sight) {
		Point shift = Point(
				sign(player.pos.x - monster.pos.x),
				sign(player.pos.y - monster.pos.y)
				);
		return Control(Control::MOVE, shift);
	}
	return Control();
}

Control calm_and_wander(Monster & monster, Game & game)
{
	return Control(Control::MOVE, Point(rand() % 3 - 1, rand() % 3 - 1));
}

Control calm_and_still(Monster & monster, Game & game)
{
	return Control();
}


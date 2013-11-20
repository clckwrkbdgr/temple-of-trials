#include "ai.h"
#include "console.h"
#include "game.h"
#include "objects.h"

Control player_control(Monster & player, Game & game)
{
	static std::map<int, Point> directions;
	if(directions.empty()) {
		directions['h'] = Point(-1,  0);
		directions['j'] = Point( 0, +1);
		directions['k'] = Point( 0, -1);
		directions['l'] = Point(+1,  0);
		directions['y'] = Point(-1, -1);
		directions['u'] = Point(+1, -1);
		directions['b'] = Point(-1, +1);
		directions['n'] = Point(+1, +1);
	}

	Console & console = Console::instance();
	while(!game.done) {
		int ch = console.draw_and_get_control(game);

		if(ch == 'q') {
			game.done = true;
		} else if(directions.count(ch) != 0) {
			return Control(Control::MOVE, directions[ch]);
		} else if(ch == 'o') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				game.message("This is not a direction.");
				continue;
			}
			return Control(Control::OPEN, directions[ch]);
		} else if(ch == 'c') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				game.message("This is not a direction.");
				continue;
			}
			return Control(Control::CLOSE, directions[ch]);
		} else {
			game.message(format("Unknown control '{0}'", char(ch)));
		}
	}
	return Control();
}

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


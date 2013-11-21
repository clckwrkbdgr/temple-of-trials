#include "ai.h"
#include "console.h"
#include "game.h"
#include "objects.h"

Controller get_controller(int ai)
{
	switch(ai) {
		case AI::PLAYER: return player_control;
		case AI::ANGRY_AND_WANDER: return angry_and_wander;
		case AI::ANGRY_AND_STILL: return angry_and_still;
		case AI::CALM_AND_WANDER: return calm_and_wander;
		case AI::CALM_AND_STILL: return calm_and_still;
		default: log("Unknown AI code: {0}", ai); break;
	}
	return 0;
}

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
		} else if(ch == '.') {
			return Control(Control::WAIT);
		} else if(directions.count(ch) != 0) {
			return Control(Control::MOVE, directions[ch]);
		} else if(ch == 's') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				game.message("This is not a direction.");
				continue;
			}
			return Control(Control::SWING, directions[ch]);
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
	return Control(Control::WAIT);
}

Control calm_and_wander(Monster & monster, Game & game)
{
	return Control(Control::MOVE, Point(rand() % 3 - 1, rand() % 3 - 1));
}

Control calm_and_still(Monster & monster, Game & game)
{
	return Control(Control::WAIT);
}


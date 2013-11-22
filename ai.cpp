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
		case AI::CALM_AND_STILL: return calm_and_still;
		default: log("Unknown AI code: {0}", ai); break;
	}
	return 0;
}


Control dummy(Monster & monster, Game & game)
{
	return Control(Control::WAIT);
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
		} else if(ch == 'Q') {
			game.done = true;
			game.player_died = true;
		} else if(ch == 'g') {
			return Control(Control::GRAB);
		} else if(ch == 'i') {
			console.draw_inventory(game, player);
			console.get_control();
			continue;
		} else if(ch == 'd') {
			int slot = console.get_inventory_slot(game, player);
			return Control(Control::DROP, slot);
		} else if(ch == '.') {
			return Control(Control::WAIT);
		} else if(directions.count(ch) != 0) {
			return Control(Control::MOVE, directions[ch]);
		} else if(ch == 's') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				console.notification("This is not a direction.");
				continue;
			}
			return Control(Control::SWING, directions[ch]);
		} else if(ch == 'o') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				console.notification("This is not a direction.");
				continue;
			}
			return Control(Control::OPEN, directions[ch]);
		} else if(ch == 'c') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				console.notification("This is not a direction.");
				continue;
			}
			return Control(Control::CLOSE, directions[ch]);
		} else {
			console.notification(format("Unknown control '{0}'", char(ch)));
		}
	}
	return Control();
}

Control angry_and_wander(Monster & monster, Game & game)
{
	const Monster & player = game.getPlayer();
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
			sign(player.pos.x - monster.pos.x),
			sign(player.pos.y - monster.pos.y)
			);
	if(1 < d && d <= monster.sight) {
		return Control(Control::MOVE, shift);
	}
	if(d == 1) {
		return Control(Control::SWING, shift);
	}
	return Control(Control::MOVE, Point(rand() % 3 - 1, rand() % 3 - 1));
}

Control angry_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.getPlayer();
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
		sign(player.pos.x - monster.pos.x),
		sign(player.pos.y - monster.pos.y)
		);
	if(1 < d && d <= monster.sight) {
		return Control(Control::MOVE, shift);
	}
	if(d == 1) {
		return Control(Control::SWING, shift);
	}
	return Control(Control::WAIT);
}

Control calm_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.getPlayer();
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
		sign(player.pos.x - monster.pos.x),
		sign(player.pos.y - monster.pos.y)
		);
	if(d == 1) {
		return Control(Control::SWING, shift);
	}
	return Control(Control::WAIT);
}


#include "ai.h"
#include "console.h"
#include "engine/game.h"
#include "engine/level.h"
#include "engine/objects.h"
#include <ncurses.h>
#include <cstdlib>

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
		if(!player.plan.empty()) {
			console.draw_game(game);
			delay(10);
			Control control = player.plan.front();
			player.plan.pop_front();
			return control;
		}
		int ch = console.draw_and_get_control(game);

		if(ch == 'q') {
			game.done = true;
		} else if(ch == 'Q') {
			game.done = true;
			game.player_died = true;
			game.message("You commited suicide.");
		} else if(ch == KEY_F(1)) {
			player.godmode = true;
		} else if(ch == 'x') {
			Point target = player.pos;
			ch = console.draw_target_mode(game, target);
			while(ch != 'x' && ch != 27 && ch != '.') {
				if(directions.count(ch) != 0) {
					Point new_target = target + directions[ch];
					if(game.level.map.valid(new_target)) {
						target = new_target;
					}
				}
				ch = console.draw_target_mode(game, target);
			}
			if(ch == '.') {
				if(game.level.map.cell_properties(target).seen_sprite == 0) {
					console.notification("You don't know how to get there.");
				} else {
					player.plan = game.level.find_path(player.pos, target);
				}
			}
			continue;
		} else if(ch == '<') {
			return Control(Control::GO_UP);
		} else if(ch == '>') {
			return Control(Control::GO_DOWN);
		} else if(ch == 'g') {
			return Control(Control::GRAB);
		} else if(ch == 'i') {
			console.draw_inventory(game, player);
			console.get_control();
			continue;
		} else if(ch == 'w') {
			int slot = console.get_inventory_slot(game, player);
			return Control(Control::WIELD, slot);
		} else if(ch == 'W') {
			int slot = console.get_inventory_slot(game, player);
			return Control(Control::WEAR, slot);
		} else if(ch == 't') {
			return Control(Control::UNWIELD);
		} else if(ch == 'T') {
			return Control(Control::TAKE_OFF);
		} else if(ch == 'e') {
			int slot = console.get_inventory_slot(game, player);
			return Control(Control::EAT, slot);
		} else if(ch == 'd') {
			int slot = console.get_inventory_slot(game, player);
			return Control(Control::DROP, slot);
		} else if(ch == '.') {
			return Control(Control::WAIT);
		} else if(directions.count(ch) != 0) {
			Point new_pos = player.pos + directions[ch];
			Door & door = find_at(game.level.doors, new_pos);
			if(door && !door.opened) {
				player.plan.push_front(Control(Control::MOVE, directions[ch]));
				return Control(Control::OPEN, directions[ch]);
			}
			if(find_at(game.level.containers, new_pos)) {
				return Control(Control::OPEN, directions[ch]);
			}
			if(find_at(game.level.fountains, new_pos)) {
				return Control(Control::DRINK, directions[ch]);
			}
			if(find_at(game.level.monsters, new_pos)) {
				return Control(Control::SWING, directions[ch]);
			}
			return Control(Control::MOVE, directions[ch]);
		} else if(ch == 'D') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				console.notification("This is not a direction.");
				continue;
			}
			return Control(Control::DRINK, directions[ch]);
		} else if(ch == 'f') {
			ch = console.draw_and_get_control(game);
			if(directions.count(ch) == 0) {
				console.notification("This is not a direction.");
				continue;
			}
			return Control(Control::FIRE, directions[ch]);
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
	const Monster & player = game.level.get_player();
	bool sees_player = game.level.map.cell_properties(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
			sign(player.pos.x - monster.pos.x),
			sign(player.pos.y - monster.pos.y)
			);
	if(sees_player && 1 < d && d <= monster.sight) {
		return Control(Control::MOVE, shift);
	}
	if(sees_player && d == 1) {
		return Control(Control::SWING, shift);
	}
	return Control(Control::MOVE, Point(rand() % 3 - 1, rand() % 3 - 1));
}

Control angry_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.level.get_player();
	bool sees_player = game.level.map.cell_properties(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
		sign(player.pos.x - monster.pos.x),
		sign(player.pos.y - monster.pos.y)
		);
	if(sees_player && 1 < d && d <= monster.sight) {
		return Control(Control::MOVE, shift);
	}
	if(sees_player && d == 1) {
		return Control(Control::SWING, shift);
	}
	return Control(Control::WAIT);
}

Control calm_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.level.get_player();
	bool sees_player = game.level.map.cell_properties(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
		sign(player.pos.x - monster.pos.x),
		sign(player.pos.y - monster.pos.y)
		);
	if(sees_player && d == 1) {
		return Control(Control::SWING, shift);
	}
	return Control(Control::WAIT);
}


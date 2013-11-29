#include "ai.h"
#include "console.h"
#include "engine/game.h"
#include "engine/level.h"
#include "engine/objects.h"
#include <ncurses.h>
#include <cstdlib>

Control player_control(Monster & player, Game & game)
{
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
				if(console.directions.count(ch) != 0) {
					Point new_target = target + console.directions[ch];
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
			return Control(Control::WIELD, console.get_inventory_slot(game, player));
		} else if(ch == 'W') {
			return Control(Control::WEAR, console.get_inventory_slot(game, player));
		} else if(ch == 't') {
			return Control(Control::UNWIELD);
		} else if(ch == 'T') {
			return Control(Control::TAKE_OFF);
		} else if(ch == 'e') {
			return Control(Control::EAT, console.get_inventory_slot(game, player));
		} else if(ch == 'd') {
			return Control(Control::DROP, console.get_inventory_slot(game, player));
		} else if(ch == '.') {
			return Control(Control::WAIT);
		} else if(console.directions.count(ch) != 0) {
			return Control(Control::SMART_MOVE, console.directions[ch]);
		} else if(ch == 'D') {
			return Control(Control::DRINK, console.draw_and_get_direction(game));
		} else if(ch == 'f') {
			return Control(Control::FIRE, console.draw_and_get_direction(game));
		} else if(ch == 's') {
			return Control(Control::SWING, console.draw_and_get_direction(game));
		} else if(ch == 'o') {
			return Control(Control::OPEN, console.draw_and_get_direction(game));
		} else if(ch == 'c') {
			return Control(Control::CLOSE, console.draw_and_get_direction(game));
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


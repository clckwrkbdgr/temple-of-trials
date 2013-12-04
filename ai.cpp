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
		switch(ch) {
			case 'Q':
				game.player_died = true;
				game.message("You commited suicide.");
			case 'q':
				game.done = true;
				break;
			case KEY_F(1):
				player.godmode = true;
				break;
			case 'x':
				player.plan = game.level.find_path(player.pos, console.target_mode(game, player.pos));
				break;
			case 'i':
				console.draw_inventory(game, player);
				console.get_control();
				break;
			case 'h': case 'j': case 'k': case 'l': case 'y': case 'u': case 'b': case 'n':
				return Control(Control::SMART_MOVE, console.directions[ch]);
			case '<': return Control(Control::GO_UP);
			case '>': return Control(Control::GO_DOWN);
			case 'g': return Control(Control::GRAB);
			case 'w': return Control(Control::WIELD, console.get_inventory_slot(game, player));
			case 'W': return Control(Control::WEAR, console.get_inventory_slot(game, player));
			case 't': return Control(Control::UNWIELD);
			case 'T': return Control(Control::TAKE_OFF);
			case 'e': return Control(Control::EAT, console.get_inventory_slot(game, player));
			case 'd': return Control(Control::DROP, console.get_inventory_slot(game, player));
			case '.': return Control(Control::WAIT);
			case 'D': return Control(Control::DRINK, console.draw_and_get_direction(game));
			case 'f': return Control(Control::FIRE, console.draw_and_get_direction(game));
			case 's': return Control(Control::SWING, console.draw_and_get_direction(game));
			case 'o': return Control(Control::OPEN, console.draw_and_get_direction(game));
			case 'c': return Control(Control::CLOSE, console.draw_and_get_direction(game));
			default: console.notification(format("Unknown control '{0}'", char(ch)));
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
	Point direction = Point(rand() % 3 - 1, rand() % 3 - 1);
	if(direction) {
		return Control(Control::MOVE, direction);
	}
	return Control(Control::WAIT);
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


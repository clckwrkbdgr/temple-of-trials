#include "ai.h"
#include "console.h"
#include "engine/game.h"
#include "engine/actions.h"
#include "engine/level.h"
#include "engine/objects.h"
#include "engine/monsters.h"
#include <ncurses.h>
#include <cstdlib>

Action * player_control(Monster & player, Game & game)
{
	Console & console = Console::instance();
	while(game.state == Game::PLAYING) {
		if(!player.plan.empty()) {
			console.draw_game(game);
			delay(10);
			Action * action = player.plan.front();
			player.plan.pop_front();
			return action;
		}
		int ch = console.draw_and_get_control(game);
		switch(ch) {
			case 'Q':
				game.state = Game::PLAYER_DIED;
				game.messages.message("You commited suicide.");
				break;
			case 'q':
				game.state = Game::SUSPENDED;
				break;
			case 'x':
				player.add_path(game.find_path(player.pos, console.target_mode(game, player.pos)));
				break;
			case 'i':
				console.draw_inventory(game, player);
				console.get_control();
				break;
			case 'h': case 'j': case 'k': case 'l': case 'y': case 'u': case 'b': case 'n':
			{
				Point shift = console.directions[ch];
				Point new_pos = player.pos + shift;
				if(find_at(game.level.monsters, new_pos).valid()) {
					return new Swing(shift);
				}
				Object & object = find_at(game.level.objects, new_pos);
				if(object.valid()) {
					if(object.openable && !object.opened) {
						player.plan.push_front(new Move(shift));
						return new Open(shift);
					}
					if(object.containable) {
						return new Open(shift);
					}
					if(object.drinkable) {
						return new Drink(shift);
					}
				}
				return new Move(shift);
			}
			case '<': return new GoUp();
			case '>': return new GoDown();
			case 'g': return new Grab();
			case 'w': return new Wield(console.get_inventory_slot(game, player));
			case 'W': return new Wear(console.get_inventory_slot(game, player));
			case 't': return new Unwield();
			case 'T': return new TakeOff();
			case 'e': return new Eat(console.get_inventory_slot(game, player));
			case 'd': return new Drop(console.get_inventory_slot(game, player));
			case '.': return new Wait();
			case 'D': return new Drink(console.draw_and_get_direction(game));
			case 'f': return new Fire(console.draw_and_get_direction(game));
			case 's': return new Swing(console.draw_and_get_direction(game));
			case 'o': return new Open(console.draw_and_get_direction(game));
			case 'c': return new Close(console.draw_and_get_direction(game));
			default: console.notification(format("Unknown control '{0}'", char(ch)));
		}
	}
	return 0;
}

Action * angry_and_wander(Monster & monster, Game & game)
{
	const Monster & player = game.get_player();
	bool sees_player = game.level.map.cell(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
			sign(player.pos.x - monster.pos.x),
			sign(player.pos.y - monster.pos.y)
			);
	if(sees_player && 1 < d && d <= monster.type->sight) {
		return new Move(shift);
	}
	if(sees_player && d == 1) {
		return new Swing(shift);
	}
	Point direction = Point(rand() % 3 - 1, rand() % 3 - 1);
	if(!direction.null()) {
		return new Move(direction);
	}
	return new Wait();
}

Action * angry_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.get_player();
	bool sees_player = game.level.map.cell(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
		sign(player.pos.x - monster.pos.x),
		sign(player.pos.y - monster.pos.y)
		);
	if(sees_player && 1 < d && d <= monster.type->sight) {
		return new Move(shift);
	}
	if(sees_player && d == 1) {
		return new Swing(shift);
	}
	return new Wait();
}

Action * calm_and_still(Monster & monster, Game & game)
{
	const Monster & player = game.get_player();
	bool sees_player = game.level.map.cell(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
		sign(player.pos.x - monster.pos.x),
		sign(player.pos.y - monster.pos.y)
		);
	if(sees_player && d == 1) {
		return new Swing(shift);
	}
	return new Wait();
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


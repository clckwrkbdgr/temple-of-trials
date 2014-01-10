#include "ai.h"
#include "console.h"
#include "engine/game.h"
#include "engine/actions.h"
#include "engine/level.h"
#include "engine/objects.h"
#include "engine/monsters.h"
#include "engine/log.h"
#include "engine/format.h"
#include <ncurses.h>
#include <cstdlib>

Action * PlayerControl::act(Monster & player, Game & game)
{
	game.events_to_messages();
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
				player.add_path(game.level().find_path(player.pos, console.target_mode(game, player.pos)));
				break;
			case 'i':
				console.draw_inventory(game, player);
				console.get_control();
				break;
			case 'h': case 'j': case 'k': case 'l': case 'y': case 'u': case 'b': case 'n':
			{
				Point shift = console.directions[ch];
				Point new_pos = player.pos + shift;
				if(find_at(game.level().monsters, new_pos).valid()) {
					return new Swing(shift);
				}
				Object & object = find_at(game.level().objects, new_pos);
				if(object.valid()) {
					if(object.type->openable && !object.opened()) {
						player.plan.push_front(new Move(shift));
						return new Open(shift);
					}
					if(object.type->containable) {
						return new Open(shift);
					}
					if(object.type->drinkable) {
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
			case 'p': return new Put(console.draw_and_get_direction(game));
			case 's': return new Swing(console.draw_and_get_direction(game));
			case 'o': return new Open(console.draw_and_get_direction(game));
			case 'c': return new Close(console.draw_and_get_direction(game));
			default: console.notification(format("Unknown control '{0}'", char(ch)));
		}
	}
	return 0;
}


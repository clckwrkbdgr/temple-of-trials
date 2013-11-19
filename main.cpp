#include "generate.h"
#include "game.h"
#include "map.h"
#include "objects.h"
#include "console.h"
#include "util.h"
#include <map>
#include <cstdlib>
#include <cstdio>

const std::string SAVEFILE = "temple.sav";

void draw_game(Console & console, const Game & game)
{
	console.clear();
	for(unsigned x = 0; x < game.map.get_width(); ++x) {
		for(unsigned y = 0; y < game.map.get_height(); ++y) {
			console.print_tile(x, y, game.map.cell(x, y).sprite);
		}
	}
	for(unsigned i = 0; i < game.doors.size(); ++i) {
		console.print_tile(game.doors[i].pos.x, game.doors[i].pos.y, game.doors[i].sprite());
	}
	for(unsigned i = 0; i < game.monsters.size(); ++i) {
		console.print_tile(game.monsters[i].pos.x, game.monsters[i].pos.y, game.monsters[i].sprite);
	}

	std::string message;
	if(game.messages.size() > 1) {
		message += format("({0}) ", game.messages.size());
	}
	if(!game.messages.empty()) {
		message += game.messages.front();
	}
	console.print_message(message);

	console.print_stat(0, format("Turns: {0}", game.turns));
}

int draw_and_get_control(Console & console, Game & game)
{
	draw_game(console, game);
	int ch = console.get_control();
	while(!game.messages.empty()) {
		if(game.messages.size() > 1) {
			switch(ch) {
				case ' ': game.messages.pop_front(); break;
				default: break;
			}
			draw_game(console, game);
			ch = console.get_control();
			continue;
		} else if(game.messages.size() == 1) {
			game.messages.pop_front();
		}
		break;
	}
	return ch;
}

Control get_player_control(Console & console, Game & game)
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

	while(!game.done) {
		int ch = draw_and_get_control(console, game);

		if(ch == 'q') {
			game.done = true;
		} else if(directions.count(ch) != 0) {
			return Control(Control::MOVE, directions[ch]);
		} else if(ch == 'o') {
			ch = draw_and_get_control(console, game);
			if(directions.count(ch) == 0) {
				game.message("This is not a direction.");
				continue;
			}
			return Control(Control::OPEN, directions[ch]);
		} else if(ch == 'c') {
			ch = draw_and_get_control(console, game);
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

Control get_ai_control(Monster & monster, Game & game)
{
	const Monster & player = game.getPlayer();
	int d = distance(monster.pos, player.pos);
	if(1 <= d && d <= monster.sight) {
		switch(monster.ai.temper) {
			case AI::TEMPER_ANGRY: {
				Point shift = Point(
						sign(player.pos.x - monster.pos.x),
						sign(player.pos.y - monster.pos.y)
						);
				return Control(Control::MOVE, shift);
			}
			default: break;
		}
	}
	switch(monster.ai.movement) {
		case AI::MOVE_WANDER:
			return Control(Control::MOVE, Point(rand() % 3 - 1, rand() % 3 - 1));
		default: break;
	}
	return Control();
}

int main()
{
	srand(time(0));
	log("Log started: " + now());
	Console console;
	Game game;
	if(game.load(SAVEFILE)) {
		if(remove(SAVEFILE.c_str()) != 0) {
			log("Error: cannot delete savefile!");
			return 1;
		}
	} else {
		generate(game);
	}

	while(!game.done) {
		for(unsigned i = 0; i < game.monsters.size(); ++i) {
			Monster & monster = game.monsters[i];
			Control control;
			if(monster.ai.faction == AI::PLAYER) {
				control = get_player_control(console, game);
			} else if(monster.ai.faction == AI::MONSTER) {
				control = get_ai_control(monster, game);
			}
			switch(control.control) {
				case Control::MOVE: game.move(monster, control.direction); break;
				case Control::OPEN: game.open(monster, control.direction); break;
				case Control::CLOSE: game.close(monster, control.direction); break;
				default: break;
			}
		}
		++game.turns;
	}
	game.save(SAVEFILE);

	log("Exiting.");
	return 0;
}

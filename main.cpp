#include "console.h"
#include "map.h"
#include "objects.h"
#include "util.h"
#include "game.h"
#include <map>
#include <cstdlib>
#include <cstdio>

const std::string SAVEFILE = "temple.sav";

void draw_game(Console & console, const Game & game)
{
	console.clear();
	for(unsigned x = 0; x < game.map.get_width(); ++x) {
		for(unsigned y = 0; y < game.map.get_height(); ++y) {
			console.print_sprite(x, y, game.map.cell(x, y).sprite);
		}
	}
	for(unsigned i = 0; i < game.doors.size(); ++i) {
		console.print_sprite(game.doors[i].pos.x, game.doors[i].pos.y, game.doors[i].sprite());
	}
	console.print_sprite(game.player.pos.x, game.player.pos.y, game.player.sprite());

	console.print_line(0, game.map.get_height(), game.message);
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
		game.generate();
	}
	while(true) {
		draw_game(console, game);
		int ch = console.get_control();
		bool ok = game.process(ch);
		if(!ok) {
			break;
		}
	}
	game.save(SAVEFILE);

	log("Exiting.");
	return 0;
}

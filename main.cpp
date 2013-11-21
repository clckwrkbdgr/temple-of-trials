#include "generate.h"
#include "ai.h"
#include "game.h"
#include "map.h"
#include "objects.h"
#include "console.h"
#include "util.h"
#include <algorithm>
#include <map>
#include <cstdlib>
#include <cstdio>

const std::string SAVEFILE = "temple.sav";

int main()
{
	srand(time(0));
	log("Log started: " + now());
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
			if(!monster.alive) {
				continue;
			}
			Controller controller = get_controller(monster.ai);
			if(!controller) {
				log(format("No controller found for AI #{0}!", monster.ai));
				continue;
			}
			Control control = controller(monster, game);
			switch(control.control) {
				case Control::MOVE: game.move(monster, control.direction); break;
				case Control::OPEN: game.open(monster, control.direction); break;
				case Control::CLOSE: game.close(monster, control.direction); break;
				case Control::SWING: game.swing(monster, control.direction); break;
				case Control::WAIT: break;
				default: log("Unknown control: {0}", control.control); break;
			}
		}
		game.monsters.erase(std::remove_if(game.monsters.begin(), game.monsters.end(), std::mem_fun_ref(&Monster::is_dead)), game.monsters.end());
		++game.turns;
	}
	game.save(SAVEFILE);

	log("Exiting.");
	return 0;
}

#include "generate.h"
#include "ai.h"
#include "game.h"
#include "map.h"
#include "objects.h"
#include "console.h"
#include "util.h"
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
			Control control;
			switch(monster.ai) {
				case AI::PLAYER: control = player_control(monster, game); break;
				case AI::ANGRY_AND_WANDER: control = angry_and_wander(monster, game); break;
				case AI::ANGRY_AND_STILL: control = angry_and_still(monster, game); break;
				case AI::CALM_AND_WANDER: control = calm_and_wander(monster, game); break;
				case AI::CALM_AND_STILL: control = calm_and_still(monster, game); break;
				default: break;
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

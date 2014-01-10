#include "generate.h"
#include "ai.h"
#include "console.h"
#include "savefile.h"
#include "engine/game.h"
#include "engine/map.h"
#include "engine/objects.h"
#include "engine/files.h"
#include "engine/util.h"
#include "engine/log.h"
#include "engine/format.h"
#include <algorithm>
#include <map>
#include <cstdlib>
#include <cstdio>

const std::string SAVEFILE = "temple.sav";

int main()
{
	srand(time(0));
	std::ofstream log_file("temple.log", std::ios::app);
	direct_log(&log_file);

	LinearDungeon dungeon;
	Game game(&dungeon);
	game.messages.log_messages = true;
	try {
		if(!file_exists(SAVEFILE)) {
			throw Reader::Exception(format("File '{0}' doesn't exists!", SAVEFILE));
		}
		std::ifstream in(SAVEFILE.c_str(), std::ios::in);
		if(!in) {
			throw Reader::Exception(format("Cannot open file '{0}' for reading!", SAVEFILE));
		}
		Reader savefile(in);
		load(savefile, game);
		if(remove(SAVEFILE.c_str()) != 0) {
			log("Error: cannot delete savefile!");
			return 1;
		}
	} catch(const Reader::Exception & e) {
		log(e.message);
		game.create_new_game();
	}

	ControllerFactory controllers;
	controllers.add_controller(AI::PLAYER, player_control);
	controllers.add_controller(AI::ANGRY_AND_WANDER, angry_and_wander);
	controllers.add_controller(AI::ANGRY_AND_STILL, angry_and_still);
	controllers.add_controller(AI::CALM_AND_STILL, calm_and_still);

	game.run(controllers);

	Console::instance().see_messages(game);
	if(game.state == Game::SUSPENDED) {
		try {
			std::ofstream out(SAVEFILE.c_str(), std::ios::out);
			if(!out) {
				throw Writer::Exception(format("Cannot open file '{0}' for writing!", SAVEFILE));
			}
			Writer savefile(out);
			save(savefile, game);
		} catch(const Writer::Exception & e) {
			log(e.message);
		}
	}

	log("Exiting.");
	return 0;
}

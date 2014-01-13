#include "generate.h"
#include "player.h"
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

bool load_game(Game & game)
{
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
			return false;
		}
	} catch(const Reader::Exception & e) {
		log(e.message);
		game.create_new_game();
	}
	return true;
}

void save(const Game & game)
{
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

int main()
{
	srand(time(0));
	std::ofstream log_file("temple.log", std::ios::app);
	direct_log(&log_file);

	TempleUI console;
	PlayerControl player(console);
	LinearDungeon game(&player);
	console.log_messages = true;
	if(!load_game(game)) {
		return 1;
	}
	game.run();
	console.see_messages(game);
	if(game.state == Game::SUSPENDED) {
		save(game);
	}

	log("Exiting.");
	return 0;
}

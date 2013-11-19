#include "game.h"
#include "files.h"

enum { SAVEFILE_VERSION = 11 };

template<class Savefile, class Game>
void store(Savefile & savefile, Game & game)
{
	savefile.version(SAVEFILE_VERSION);
	savefile.newline();

	if(savefile.version() > 8) {
		savefile.store(game.turns);
		savefile.newline();
	}

	savefile.size_of(game.map);
	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < game.map.get_height(); ++y) {
		for(unsigned x = 0; x < game.map.get_width(); ++x) {
			savefile.store(game.map.cell(x, y).sprite).store(game.map.cell(x, y).passable);
			savefile.check("map cell");
		}
		savefile.newline();
	}
	savefile.newline();

	savefile.size_of(game.monsters);
	savefile.newline();
	savefile.check("monster count");
	for(unsigned i = 0; i < game.monsters.size(); ++i) {
		savefile.store(game.monsters[i].pos.x).store(game.monsters[i].pos.y).store(game.monsters[i].sprite);
		if(savefile.version() >= 10) {
			savefile.store(game.monsters[i].sight);
		}
		savefile.store(game.monsters[i].ai.faction).store(game.monsters[i].ai.movement);
		if(savefile.version() >= 11) {
			savefile.store(game.monsters[i].ai.temper);
		}
		savefile.store(game.monsters[i].name);
		savefile.newline();
		savefile.check("monster");
	}
	savefile.newline();

	savefile.size_of(game.doors);
	savefile.newline();
	savefile.check("door count");
	for(unsigned i = 0; i < game.doors.size(); ++i) {
		savefile.store(game.doors[i].pos.x).store(game.doors[i].pos.y);
		savefile.store(game.doors[i].opened_sprite).store(game.doors[i].closed_sprite);
		savefile.store(game.doors[i].opened);
		savefile.newline();
		savefile.check("door");
	}
	savefile.newline();
}

bool Game::load(const std::string & filename)
{
	try {
		Reader savefile(filename);
		store(savefile, *this);
	} catch(const Reader::Exception & e) {
		log(e.message);
		return false;
	}
	return true;
}

bool Game::save(const std::string & filename) const
{
	try {
		Writer savefile(filename);
		store(savefile, *this);
	} catch(const Writer::Exception & e) {
		log(e.message);
		return false;
	}
	return true;
}


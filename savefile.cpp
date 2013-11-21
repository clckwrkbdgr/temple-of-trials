#include "ai.h"
#include "game.h"
#include "files.h"

enum { SAVEFILE_VERSION = 15 };

void store_ai(Reader & savefile, Monster & monster)
{
	int faction, movement, temper;
	savefile.store(faction).store(movement);
	if(savefile.version() >= 11) {
		savefile.store(temper);
	}
	if(faction == 0) {
		monster.ai = AI::PLAYER;
	} else {
		if(movement == 0 && temper == 0) {
			monster.ai = AI::CALM_AND_STILL;
		} else if(movement == 0 && temper == 1) {
			monster.ai = AI::ANGRY_AND_STILL;
		} else if(movement == 1) {
			monster.ai = AI::ANGRY_AND_WANDER;
		}
	}
}

void store_ai(Writer & savefile, const Monster & monster)
{
	int faction = 0, movement = 0, temper = 0;
	switch(monster.ai) {
		case AI::PLAYER: faction = 0; break;
		case AI::ANGRY_AND_WANDER: movement = 1; temper = 1; break;
		case AI::ANGRY_AND_STILL: movement = 0; temper = 1; break;
		case AI::CALM_AND_STILL: movement = 0; temper = 0; break;
	}
	savefile.store(faction).store(movement);
	if(savefile.version() >= 11) {
		savefile.store(temper);
	}
}

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
		if(savefile.version() >= 13) {
			savefile.store(game.monsters[i].hp);
		}
		if(savefile.version() <= 11) {
			store_ai(savefile, game.monsters[i]);
		} else {
			savefile.store(game.monsters[i].ai);
		}
		savefile.store(game.monsters[i].name);
		if(savefile.version() >= 15) {
			savefile.newline();
			savefile.size_of(game.monsters[i].inventory);
			savefile.newline();
			savefile.check("inventory size");
			for(unsigned i = 0; i < game.monsters[i].inventory.size(); ++i) {
				savefile.store(game.monsters[i].inventory[i].pos.x).store(game.monsters[i].inventory[i].pos.y);
				savefile.store(game.monsters[i].inventory[i].sprite).store(game.monsters[i].inventory[i].name);
				savefile.newline();
				savefile.check("inventory item");
			}
			savefile.newline();
		}
		savefile.check("monster");
	}
	savefile.newline();

	if(savefile.version() >= 14) {
		savefile.size_of(game.items);
		savefile.newline();
		savefile.check("item count");
		for(unsigned i = 0; i < game.items.size(); ++i) {
			savefile.store(game.items[i].pos.x).store(game.items[i].pos.y);
			savefile.store(game.items[i].sprite).store(game.items[i].name);
			savefile.newline();
			savefile.check("item");
		}
		savefile.newline();
	}

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


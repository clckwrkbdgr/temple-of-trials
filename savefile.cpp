#include "ai.h"
#include "game.h"
#include "files.h"

enum { SAVEFILE_VERSION = 17 };

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
	for(decltype(game.monsters.begin()) monster = game.monsters.begin(); monster != game.monsters.end(); ++monster) {
		savefile.store(monster->pos.x).store(monster->pos.y).store(monster->sprite);
		if(savefile.version() >= 10) {
			savefile.store(monster->sight);
		}
		if(savefile.version() >= 13) {
			savefile.store(monster->hp);
		}
		if(savefile.version() >= 17) {
			savefile.store(monster->wielded);
		}
		if(savefile.version() <= 11) {
			store_ai(savefile, *monster);
		} else {
			savefile.store(monster->ai);
		}
		savefile.store(monster->name);
		if(savefile.version() >= 15) {
			savefile.newline();
			savefile.size_of(monster->inventory);
			savefile.newline();
			savefile.check("inventory size");
			for(decltype(monster->inventory.begin()) item = monster->inventory.begin(); item != monster->inventory.end(); ++item) {
				savefile.store(item->pos.x).store(item->pos.y);
				savefile.store(item->sprite).store(item->name);
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
		for(decltype(game.items.begin()) item = game.items.begin(); item != game.items.end(); ++item) {
			savefile.store(item->pos.x).store(item->pos.y);
			savefile.store(item->sprite).store(item->name);
			savefile.newline();
			savefile.check("item");
		}
		savefile.newline();
	}

	if(savefile.version() >= 16) {
		savefile.size_of(game.containers);
		savefile.newline();
		savefile.check("container count");
		for(decltype(game.containers.begin()) container = game.containers.begin(); container != game.containers.end(); ++container) {
			savefile.store(container->pos.x).store(container->pos.y);
			savefile.store(container->sprite).store(container->name);
			savefile.newline();
			savefile.size_of(container->items);
			savefile.newline();
			savefile.check("container size");
			for(decltype(container->items.begin()) item = container->items.begin(); item != container->items.end(); ++item) {
				savefile.store(item->pos.x).store(item->pos.y);
				savefile.store(item->sprite).store(item->name);
				savefile.newline();
				savefile.check("container item");
			}
			savefile.newline();
			savefile.check("container");
		}
		savefile.newline();
	}

	savefile.size_of(game.doors);
	savefile.newline();
	savefile.check("door count");
	for(decltype(game.doors.begin()) door = game.doors.begin(); door != game.doors.end(); ++door) {
		savefile.store(door->pos.x).store(door->pos.y);
		savefile.store(door->opened_sprite).store(door->closed_sprite);
		savefile.store(door->opened);
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


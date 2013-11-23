#include "ai.h"
#include "game.h"
#include "files.h"

enum { SAVEFILE_VERSION = 22 };

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

SAVEFILE_STORE_EXT(Cell, cell)
{
	savefile.store(cell.sprite).store(cell.passable);
}

SAVEFILE_STORE_EXT(Item, item)
{
	savefile.store(item.pos.x).store(item.pos.y);
	savefile.store(item.sprite).store(item.name);
	if(savefile.version() >= 19) {
		savefile.store(item.damage);
	}
	if(savefile.version() >= 21) {
		savefile.store(item.wearable);
	}
	if(savefile.version() >= 22) {
		savefile.store(item.defence);
	}
}

SAVEFILE_STORE_EXT(Container, container)
{
	savefile.store(container.pos.x).store(container.pos.y);
	savefile.store(container.sprite).store(container.name);
	savefile.newline();
	savefile.store(container.items, "container item");
}

SAVEFILE_STORE_EXT(Door, door)
{
	savefile.store(door.pos.x).store(door.pos.y);
	savefile.store(door.opened_sprite).store(door.closed_sprite);
	savefile.store(door.opened);
}

SAVEFILE_STORE_EXT(Monster, monster)
{
	savefile.store(monster.pos.x).store(monster.pos.y).store(monster.sprite);
	if(savefile.version() >= 10) {
		savefile.store(monster.sight);
	}
	if(savefile.version() >= 13) {
		savefile.store(monster.hp);
	}
	if(savefile.version() >= 17) {
		savefile.store(monster.wielded);
	}
	if(savefile.version() >= 20) {
		savefile.store(monster.worn);
	}
	if(savefile.version() >= 18) {
		savefile.store(monster.hit_strength);
	}
	if(savefile.version() <= 11) {
		store_ai(savefile, monster);
	} else {
		savefile.store(monster.ai);
	}
	savefile.store(monster.name);
	if(savefile.version() >= 15) {
		savefile.newline();
		savefile.store(monster.inventory, "inventory item");
	}
}

SAVEFILE_STORE_EXT(Game, game)
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
			store_cell(savefile, game.map.cell(x, y));
			savefile.check("map cell");
		}
		savefile.newline();
	}
	savefile.newline();

	savefile.store(game.monsters, "monster");
	savefile.newline();

	if(savefile.version() >= 14) {
		savefile.store(game.items, "item");
		savefile.newline();
	}

	if(savefile.version() >= 16) {
		savefile.store(game.containers, "container");
		savefile.newline();
	}

	savefile.store(game.doors, "door");
	savefile.newline();
}

bool Game::load(const std::string & filename)
{
	try {
		Reader savefile(filename);
		store_ext(savefile, *this);
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
		store_ext(savefile, *this);
	} catch(const Writer::Exception & e) {
		log(e.message);
		return false;
	}
	return true;
}


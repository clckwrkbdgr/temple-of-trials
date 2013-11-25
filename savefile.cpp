#include "ai.h"
#include "game.h"
#include "files.h"

enum { SAVEFILE_MAJOR_VERSION = 23, SAVEFILE_MINOR_VERSION = 0 };

SAVEFILE_STORE_EXT(CellType, celltype)
{
	savefile.store(celltype.sprite).store(celltype.passable);
}

SAVEFILE_STORE_EXT(Item, item)
{
	savefile.store(item.pos.x).store(item.pos.y);
	savefile.store(item.sprite).store(item.name);
	savefile.store(item.damage);
	savefile.store(item.wearable);
	savefile.store(item.defence);
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
	savefile.store(monster.sight);
	savefile.store(monster.hp);
	savefile.store(monster.wielded);
	savefile.store(monster.worn);
	savefile.store(monster.hit_strength);
	savefile.store(monster.ai);
	savefile.store(monster.name);
	savefile.newline();
	savefile.store(monster.inventory, "inventory item");
}

SAVEFILE_STORE_EXT(Game, game)
{
	savefile.version(SAVEFILE_MAJOR_VERSION, SAVEFILE_MINOR_VERSION);
	savefile.newline();

	savefile.store(game.turns);
	savefile.newline();

	savefile.size_of(game.map);
	savefile.newline();
	savefile.store(game.map.celltypes, "celltype");
	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < game.map.height; ++y) {
		for(unsigned x = 0; x < game.map.width; ++x) {
			savefile.store(game.map.celltype(x, y));
			savefile.check("map cell");
		}
		savefile.newline();
	}
	savefile.newline();

	savefile.store(game.monsters, "monster");
	savefile.newline();

	savefile.store(game.items, "item");
	savefile.newline();

	savefile.store(game.containers, "container");
	savefile.newline();

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


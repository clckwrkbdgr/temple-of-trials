#include "ai.h"
#include "game.h"
#include "files.h"

enum { SAVEFILE_MAJOR_VERSION = 23, SAVEFILE_MINOR_VERSION = 14 };

SAVEFILE_STORE_EXT(CellType, celltype)
{
	savefile.store(celltype.sprite).store(celltype.passable);
	if(savefile.version() > 0) {
		savefile.store(celltype.hurts);
	}
	if(savefile.version() >= 7) {
		savefile.store(celltype.transparent);
	}
	if(savefile.version() >= 12) {
		savefile.store(celltype.name);
	}
}

SAVEFILE_STORE_EXT(Cell, cell)
{
	savefile.store(cell.type);
	if(savefile.version() >= 11) {
		savefile.store(cell.seen_sprite);
	}
}

SAVEFILE_STORE_EXT(Item, item)
{
	savefile.store(item.pos.x).store(item.pos.y);
	savefile.store(item.sprite).store(item.name);
	savefile.store(item.damage);
	savefile.store(item.wearable);
	savefile.store(item.defence);
	if(savefile.version() >= 6) {
		savefile.store(item.edible).store(item.antidote);
	}
	if(savefile.version() >= 8) {
		savefile.store(item.healing);
	}
	if(savefile.version() >= 9) {
		savefile.store(item.quest);
	}
}

SAVEFILE_STORE_EXT(Container, container)
{
	savefile.store(container.pos.x).store(container.pos.y);
	savefile.store(container.sprite).store(container.name);
	savefile.newline();
	savefile.store(container.items, "container item");
}

SAVEFILE_STORE_EXT(Fountain, fountain)
{
	savefile.store(fountain.pos.x).store(fountain.pos.y);
	savefile.store(fountain.sprite).store(fountain.name);
}

SAVEFILE_STORE_EXT(Stairs, stairs)
{
	savefile.store(stairs.pos.x).store(stairs.pos.y);
	savefile.store(stairs.sprite).store(stairs.name);
	if(savefile.version() >= 13) {
		savefile.store(stairs.up_destination);
		savefile.store(stairs.down_destination);
	}
}

SAVEFILE_STORE_EXT(Door, door)
{
	savefile.store(door.pos.x).store(door.pos.y);
	savefile.store(door.opened_sprite).store(door.closed_sprite);
	if(savefile.version() >= 12) {
		savefile.store(door.name);
	}
	savefile.store(door.opened);
}

SAVEFILE_STORE_EXT(Monster, monster)
{
	savefile.store(monster.pos.x).store(monster.pos.y).store(monster.sprite);
	savefile.store(monster.sight);
	savefile.store(monster.hp);
	if(savefile.version() >= 3) {
		savefile.store(monster.max_hp);
	}
	savefile.store(monster.wielded);
	savefile.store(monster.worn);
	savefile.store(monster.hit_strength);
	if(savefile.version() >= 4) {
		savefile.store(monster.poisonous);
	}
	if(savefile.version() >= 5) {
		savefile.store(monster.poisoning);
	}
	savefile.store(monster.ai);
	savefile.store(monster.name);
	savefile.newline();
	savefile.store(monster.inventory, "inventory item");
}

SAVEFILE_STORE_EXT(Game, game)
{
	savefile.version(SAVEFILE_MAJOR_VERSION, SAVEFILE_MINOR_VERSION);
	savefile.newline();

	if(savefile.version() >= 14) {
		savefile.store(game.current_level);
	}
	savefile.store(game.turns);
	savefile.newline();

	savefile.size_of(game.map);
	savefile.newline();
	savefile.store(game.map.celltypes, "celltype");
	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < game.map.height; ++y) {
		for(unsigned x = 0; x < game.map.width; ++x) {
			store_ext(savefile, game.map.cell_properties(x, y));
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

	if(savefile.version() >= 2) {
		savefile.store(game.fountains, "fountain");
		savefile.newline();
	}

	if(savefile.version() >= 10) {
		savefile.store(game.stairs, "stairs");
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


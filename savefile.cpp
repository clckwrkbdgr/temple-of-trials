#include "ai.h"
#include "game.h"
#include "files.h"

enum { SAVEFILE_MAJOR_VERSION = 23, SAVEFILE_MINOR_VERSION = 18 };

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

SAVEFILE_STORE_EXT(Trap, trap)
{
	savefile.store(trap.pos.x).store(trap.pos.y);
	savefile.store(trap.sprite).store(trap.name);
	if(savefile.version() >= 17) {
		savefile.store(trap.triggered);
	}
	if(savefile.version() >= 18) {
		store_ext(savefile, trap.bolt);
	}
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

SAVEFILE_STORE_EXT(Level, level)
{
	savefile.size_of(level.map);
	savefile.newline();
	savefile.store(level.map.celltypes, "celltype");
	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < level.map.height; ++y) {
		for(unsigned x = 0; x < level.map.width; ++x) {
			store_ext(savefile, level.map.cell_properties(x, y));
			savefile.check("map cell");
		}
		savefile.newline();
	}
	savefile.newline();

	savefile.store(level.monsters, "monster");
	savefile.newline();

	savefile.store(level.items, "item");
	savefile.newline();

	savefile.store(level.containers, "container");
	savefile.newline();

	if(savefile.version() >= 2) {
		savefile.store(level.fountains, "fountain");
		savefile.newline();
	}

	if(savefile.version() >= 10) {
		savefile.store(level.stairs, "stairs");
		savefile.newline();
	}

	if(savefile.version() >= 16) {
		savefile.store(level.traps, "trap");
		savefile.newline();
	}

	savefile.store(level.doors, "door");
}

void store_ext(Writer & savefile, const std::map<int, Level> & saved_levels)
{
	savefile.store(unsigned(saved_levels.size()));
	savefile.newline();
	std::map<int, Level>::const_iterator i;
	for(i = saved_levels.begin(); i != saved_levels.end(); ++i) {
		int level_index = i->first;
		const Level & level = i->second;
		savefile.store(level_index);
		savefile.newline();
		store_ext(savefile, level);
		savefile.newline();
	}
}

void store_ext(Reader & savefile, std::map<int, Level> & saved_levels)
{
	int count;
	savefile.store(count);
	savefile.newline();
	while(count --> 0) {
		int level_index;
		savefile.store(level_index);
		savefile.newline();
		store_ext(savefile, saved_levels[level_index]);
		savefile.newline();
	}
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

	store_ext(savefile, game.level);
	savefile.newline();

	if(savefile.version() >= 15) {
		store_ext(savefile, game.saved_levels);
	}
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


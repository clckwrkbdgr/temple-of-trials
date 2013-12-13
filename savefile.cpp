#include "ai.h"
#include "engine/game.h"
#include "engine/files.h"

enum { SAVEFILE_MAJOR_VERSION = 24, SAVEFILE_MINOR_VERSION = 0 };

SAVEFILE_STORE_EXT(CellType, celltype)
{
	savefile.store(celltype.sprite).store(celltype.passable);
	savefile.store(celltype.hurts).store(celltype.transparent).store(celltype.name);
}

SAVEFILE_STORE_EXT(Cell, cell)
{
	savefile.store(cell.type).store(cell.seen_sprite);
}

SAVEFILE_STORE_EXT(Item, item)
{
	savefile.store(item.pos.x).store(item.pos.y);
	savefile.store(item.sprite).store(item.name);
	savefile.store(item.damage).store(item.wearable).store(item.defence);
	savefile.store(item.edible).store(item.antidote).store(item.healing);
	savefile.store(item.quest);
}

SAVEFILE_STORE_EXT(Object, object)
{
	savefile.store(object.pos.x).store(object.pos.y);
	savefile.store(object.sprite).store(object.name);
	savefile.newline();
	savefile.store(object.items, "object item");
}

SAVEFILE_STORE_EXT(Trap, trap)
{
	savefile.store(trap.pos.x).store(trap.pos.y);
	savefile.store(trap.sprite).store(trap.name);
	savefile.store(trap.triggered);
	store_ext(savefile, trap.bolt);
}

SAVEFILE_STORE_EXT(Stairs, stairs)
{
	savefile.store(stairs.pos.x).store(stairs.pos.y);
	savefile.store(stairs.sprite).store(stairs.name);
	savefile.store(stairs.up_destination).store(stairs.down_destination);
}

SAVEFILE_STORE_EXT(Door, door)
{
	savefile.store(door.pos.x).store(door.pos.y);
	savefile.store(door.opened_sprite).store(door.closed_sprite);
	savefile.store(door.name).store(door.opened);
}

SAVEFILE_STORE_EXT(Monster, monster)
{
	savefile.store(monster.pos.x).store(monster.pos.y).store(monster.sprite);
	savefile.store(monster.sight);
	savefile.store(monster.hp).store(monster.max_hp);
	savefile.store(monster.wielded);
	savefile.store(monster.worn);
	savefile.store(monster.hit_strength);
	savefile.store(monster.poisonous).store(monster.poisoning);
	savefile.store(monster.ai);
	savefile.store(monster.name);
	savefile.store(monster.faction);
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

	savefile.store(level.objects, "object");
	savefile.newline();

	savefile.store(level.stairs, "stairs");
	savefile.newline();

	savefile.store(level.traps, "trap");
	savefile.newline();

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

	savefile.store(game.current_level);
	savefile.store(game.turns);
	savefile.newline();

	store_ext(savefile, game.level);
	savefile.newline();

	store_ext(savefile, game.saved_levels);
	savefile.newline();
}

void load(Reader & savefile, Game & game)
{
	store_ext(savefile, game);
}

void save(Writer & savefile, const Game & game)
{
	store_ext(savefile, game);
}

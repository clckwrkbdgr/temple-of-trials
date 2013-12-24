#include "ai.h"
#include "engine/game.h"
#include "engine/files.h"

enum { SAVEFILE_MAJOR_VERSION = 26, SAVEFILE_MINOR_VERSION = 0 };

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
	savefile.store(item.pos).store(item.pos);
	savefile.store(item.sprite).store(item.name);
	savefile.store(item.damage).store(item.wearable).store(item.defence);
	savefile.store(item.edible).store(item.antidote).store(item.healing);
	savefile.store(item.quest);
	savefile.store(item.key_type);
}

SAVEFILE_STORE_EXT(Object, object)
{
	savefile.store(object.pos);
	savefile.store(object.sprite).store(object.opened_sprite).store(object.name);
	savefile.store(object.passable).store(object.transparent);
	savefile.store(object.containable).store(object.drinkable);
	savefile.store(object.transporting).store(object.triggerable);
	savefile.store(object.openable).store(object.opened);
	savefile.store(object.up_destination).store(object.down_destination);
	savefile.store(object.locked).store(object.lock_type);
	savefile.newline();
	savefile.store(object.items, "object item");
}

SAVEFILE_STORE_EXT(Inventory, inventory)
{
	savefile.store(inventory.wielded);
	savefile.store(inventory.worn);
	savefile.newline();
	savefile.store(inventory.items, "inventory item");
}

SAVEFILE_STORE_EXT(Monster, monster)
{
	savefile.store(monster.pos).store(monster.sprite);
	savefile.store(monster.sight);
	savefile.store(monster.hp).store(monster.max_hp);
	savefile.store(monster.hit_strength);
	savefile.store(monster.poisonous).store(monster.poisoning);
	savefile.store(monster.ai);
	savefile.store(monster.name);
	savefile.store(monster.faction);
	savefile.newline();
	savefile.store(monster.inventory);
}

SAVEFILE_STORE_EXT(Level, level)
{
	savefile.size_of(level.map);
	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < level.map.height; ++y) {
		for(unsigned x = 0; x < level.map.width; ++x) {
			savefile.store(level.map.cell(x, y));
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
}

SAVEFILE_STORE_EXT(Game, game)
{
	savefile.version(SAVEFILE_MAJOR_VERSION, SAVEFILE_MINOR_VERSION);
	savefile.newline();

	savefile.store(game.current_level);
	savefile.store(game.turns);
	savefile.newline();

	savefile.store(game.cell_types, "celltype");
	savefile.newline();

	savefile.store(game.level);
	savefile.newline();

	savefile.store(game.saved_levels, "levels");
	savefile.newline();
}

void load(Reader & savefile, Game & game)
{
	savefile.store(game);
}

void save(Writer & savefile, const Game & game)
{
	savefile.store(game);
}

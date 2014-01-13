#include "engine/game.h"
#include "engine/files.h"
using namespace Chthon;

enum { SAVEFILE_MAJOR_VERSION = 35, SAVEFILE_MINOR_VERSION = 0 };

SAVEFILE_STORE(CellType, celltype)
{
	savefile.store(celltype.id).store(celltype.sprite).store(celltype.passable);
	savefile.store(celltype.hurts).store(celltype.transparent).store(celltype.name);
}

SAVEFILE_STORE(Cell, cell)
{
	savefile.store_type(cell.type).store(cell.seen_sprite);
}

SAVEFILE_STORE(Item, item)
{
	savefile.store_type(item.type);
	if(savefile.version() >= 1) {
		savefile.store_type(item.full_type).store_type(item.empty_type);
	}
	savefile.store(item.pos).store(item.key_type);
}

SAVEFILE_STORE(Object, object)
{
	savefile.store_type(object.type);
	savefile.store_type(object.closed_type);
	savefile.store_type(object.opened_type);
	savefile.store(object.pos);
	savefile.store(object.up_destination).store(object.down_destination);
	savefile.store(object.locked).store(object.lock_type);
	savefile.newline();
	savefile.store(object.items, "object item");
}

SAVEFILE_STORE(Inventory, inventory)
{
	savefile.store(inventory.wielded);
	savefile.store(inventory.worn);
	savefile.newline();
	savefile.store(inventory.items, "inventory item");
}

SAVEFILE_STORE(Monster, monster)
{
	savefile.store_type(monster.type);
	savefile.store(monster.pos).store(monster.hp);
	savefile.store(monster.poisoning);
	savefile.newline();
	savefile.store(monster.inventory);
}

SAVEFILE_STORE(Level, level)
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

SAVEFILE_STORE(Game, game)
{
	//savefile.add_type_registry(game.cell_types);
	//savefile.add_type_registry(game.monster_types);
	//savefile.add_type_registry(game.object_types);
	//savefile.add_type_registry(game.item_types);

	savefile.version(SAVEFILE_MAJOR_VERSION, SAVEFILE_MINOR_VERSION);
	savefile.newline();

	savefile.store(game.current_level_index);
	savefile.store(game.turns);
	savefile.newline();

	savefile.store(game.levels, "levels");
	savefile.newline();
}

void load(Reader & savefile, Game & game)
{
	store(savefile, game);
}

void save(Writer & savefile, const Game & game)
{
	store(savefile, game);
}

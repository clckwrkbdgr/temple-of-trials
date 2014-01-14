#include <chthon/game.h>
#include <chthon/level.h>
#include <chthon/items.h>
#include <chthon/objects.h>
#include <chthon/monsters.h>
#include <chthon/cell.h>
#include <chthon/files.h>
#include <chthon/types.h>
using namespace Chthon;

enum { SAVEFILE_MAJOR_VERSION = 35, SAVEFILE_MINOR_VERSION = 0 };

static const TypeRegistry<std::string, Cell> * cell_types = 0;
static const TypeRegistry<std::string, Monster> * monster_types = 0;
static const TypeRegistry<std::string, Object> * object_types = 0;
static const TypeRegistry<std::string, Item> * item_types = 0;
const TypeRegistry<std::string, Cell> * get_registry(const CellType *) { return cell_types; }
const TypeRegistry<std::string, Monster> * get_registry(const MonsterType *) { return monster_types; }
const TypeRegistry<std::string, Object> * get_registry(const ObjectType *) { return object_types; }
const TypeRegistry<std::string, Item> * get_registry(const ItemType *) { return item_types; }

SAVEFILE_STORE(Point, point)
{
	savefile.store(point.x).store(point.y);
}

template<class Savefile, class T>
void store_type(Savefile & savefile, TypePtr<T> & type)
{
	std::string type_id;
	savefile.store(type_id);
	type = TypePtr<T>(get_registry((T*)0)->get(type_id));
	savefile.check(type_id + " type");
}
template<class Savefile, class T>
void store_type(Savefile & savefile, const TypePtr<T> & type)
{
	savefile.store(type->id);
	savefile.check(type->id + " type");
}

FORWARD_DECLARE_SAVEFILE_STORE(Monster);
FORWARD_DECLARE_SAVEFILE_STORE(Item);
FORWARD_DECLARE_SAVEFILE_STORE(Object);

template<class Savefile, class T>
void store(Savefile & savefile, std::vector<T> & v, const std::string & name)
{
	unsigned size = 0;
	savefile.store(size);
	v.resize(size);
	savefile.newline().check(name + " count");
	for(decltype(v.begin()) item = v.begin(); item != v.end(); ++item) {
		store(savefile, *item);
		savefile.newline().check(name);
	}
}

template<class Savefile, class T>
void store(Savefile & savefile, const std::vector<T> & v, const std::string & name)
{
	savefile.store(unsigned(v.size()));
	savefile.newline().check(name + " count");
	for(decltype(v.begin()) item = v.begin(); item != v.end(); ++item) {
		store(savefile, *item);
		savefile.newline().check(name);
	}
}

FORWARD_DECLARE_SAVEFILE_STORE(Cell);

template<class Savefile, class T>
void store(Savefile & savefile, Map<T> & map)
{
	unsigned width = 0, height = 0;
	savefile.store(width).store(height);
	map = Map<T>(width, height);

	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < map.height; ++y) {
		for(unsigned x = 0; x < map.width; ++x) {
			store(savefile, map.cell(x, y));
			savefile.check("map cell");
		}
		savefile.newline();
	}
}

template<class Savefile, class T>
void store(Savefile & savefile, const Map<T> & map)
{
	savefile.store(map.width).store(map.height);

	savefile.newline();
	savefile.check("map size");
	for(unsigned y = 0; y < map.height; ++y) {
		for(unsigned x = 0; x < map.width; ++x) {
			store(savefile, map.cell(x, y));
			savefile.check("map cell");
		}
		savefile.newline();
	}
}

SAVEFILE_STORE(Cell, cell)
{
	store_type(savefile, cell.type);
	savefile.store(cell.seen_sprite);
}

SAVEFILE_STORE(Item, item)
{
	store_type(savefile, item.type);
	store_type(savefile, item.full_type);
	store_type(savefile, item.empty_type);
	store(savefile, item.pos);
	savefile.store(item.key_type);
}

SAVEFILE_STORE(Object, object)
{
	store_type(savefile, object.type);
	store_type(savefile, object.closed_type);
	store_type(savefile, object.opened_type);
	store(savefile, object.pos);
	savefile.store(object.up_destination).store(object.down_destination);
	savefile.store(object.locked).store(object.lock_type);
	savefile.newline();
	store(savefile, object.items, "object item");
}

SAVEFILE_STORE(Inventory, inventory)
{
	savefile.store(inventory.wielded);
	savefile.store(inventory.worn);
	savefile.newline();
	store(savefile, inventory.items, "inventory item");
}

SAVEFILE_STORE(Monster, monster)
{
	store_type(savefile, monster.type);
	store(savefile, monster.pos);
	savefile.store(monster.hp);
	savefile.store(monster.poisoning);
	savefile.newline();
	store(savefile, monster.inventory);
}

SAVEFILE_STORE(Level, level)
{
	store(savefile, level.map);
	savefile.newline();

	store(savefile, level.monsters, "monster");
	savefile.newline();

	store(savefile, level.items, "item");
	savefile.newline();

	store(savefile, level.objects, "object");
}

template<class Savefile, class K, class V>
void store(Savefile & savefile, std::map<K, V> & map, const std::string & name)
{
	int count;
	savefile.store(count).newline().check(name + " count");
	while(count --> 0) {
		K key;
		store(savefile, key);
		store(savefile, map[key]);
		savefile.newline().check(name);
	}
}
template<class Savefile, class K, class V>
void store(Savefile & savefile, const std::map<K, V> & map, const std::string & name)
{
	savefile.store(unsigned(map.size())).newline().check(name + " count");
	typename std::map<K, V>::const_iterator i;
	for(i = map.begin(); i != map.end(); ++i) {
		store(savefile, i->first);
		store(savefile, i->second);
		savefile.newline().check(name);
	}
}

SAVEFILE_STORE(Game, game)
{
	savefile.version(SAVEFILE_MAJOR_VERSION, SAVEFILE_MINOR_VERSION);
	savefile.newline();

	savefile.store(game.current_level_index);
	savefile.store(game.turns);
	savefile.newline();

	store(savefile, game.levels, "levels");
	savefile.newline();
}

void load(Reader & savefile, Game & game)
{
	cell_types = &game.cell_types;
	monster_types = &game.monster_types;
	object_types = &game.object_types;
	item_types = &game.item_types;
	store(savefile, game);
}

void save(Writer & savefile, const Game & game)
{
	store(savefile, game);
}

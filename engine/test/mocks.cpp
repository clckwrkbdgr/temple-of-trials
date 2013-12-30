#include "mocks.h"

namespace GameMocks {

DummyDungeon::DummyDungeon() : Dungeon() {}
DummyDungeon::~DummyDungeon() {}
void DummyDungeon::generate(Level & /*level*/, int /*level_index*/) {}
void DummyDungeon::create_types(Game & /*game*/) {}

TestDungeon::TestDungeon(const Point & player_pos1, const Point & player_pos2)
	: generated(false), pos1(player_pos1), pos2(player_pos2)
{
	player_one = MonsterType::Builder("player_one").sprite(1).faction(Monster::PLAYER);
	player_two = MonsterType::Builder("player_two").sprite(2).faction(Monster::PLAYER);
}
void TestDungeon::create_types(Game & game) { }
void TestDungeon::generate(Level & level, int level_index)
{
	generated = true;
	level = Level(4, 4);
	if(level_index == 1) {
		level.monsters.push_back(Monster::Builder(&player_one).pos(pos1));
	} else {
		level.monsters.push_back(Monster::Builder(&player_two).pos(pos2));
	}
}


GameWithDummyWieldingAndWearing::GameWithDummyWieldingAndWearing()
	: game(&dungeon)
{
	game.level().map = Map(2, 3);
	game.cell_types.insert(CellType::Builder("floor").passable(true).transparent(true).name("floor"));
	dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
	stub_type = game.monster_types.insert(MonsterType::Builder("stub").name("stub").max_hp(100));
	game.object_types.insert(ObjectType::Builder("door").name("door"));
	game.object_types.insert(ObjectType::Builder("pot").name("pot").containable());
	game.object_types.insert(ObjectType::Builder("well").name("well").drinkable());
	const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("armor"));
	const ItemType * spear = game.item_types.insert(ItemType::Builder("spear").sprite(2).damage(3).name("spear"));
	game.item_types.insert(ItemType::Builder("item").sprite(1).name("item"));
	game.item_types.insert(ItemType::Builder("quest_item").sprite(1).name("item").quest());
	game.item_types.insert(ItemType::Builder("stub").sprite(2).name("stub"));
	const ItemType * jacket = game.item_types.insert(ItemType::Builder("jacket").sprite(1).name("jacket").wearable());
	const ItemType * full_flask = game.item_types.insert(ItemType::Builder("full_flask").sprite(1).name("water flask"));
	const ItemType * empty_flask = game.item_types.insert(ItemType::Builder("empty_flask").sprite(2).name("empty flask"));

	game.level().map.fill(game.cell_types.get("floor"));
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 2)).
			item(Item(spear)).item(Item(armor)).wield(0).wear(1).
			item(jacket).item(Item::Builder(full_flask, empty_flask).make_empty()));
}
Monster & GameWithDummyWieldingAndWearing::dummy() { return game.level().monsters[0]; }

GameWithDummyWithItems::GameWithDummyWithItems()
	: game(&dungeon)
{
	game.level().map = Map(2, 3);
	game.cell_types.insert(CellType::Builder("floor").passable(true).transparent(true).name("floor"));
	dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
	const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("armor"));
	const ItemType * spear = game.item_types.insert(ItemType::Builder("spear").sprite(2).damage(3).name("spear"));
	const ItemType * pot = game.item_types.insert(ItemType::Builder("pot").sprite(1).name("pot"));

	game.level().map.fill(game.cell_types.get("floor"));
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 2)).item(spear).item(armor).item(pot).item(pot));
	dummy().inventory.take_item(2);
}
Monster & GameWithDummyWithItems::dummy() { return game.level().monsters[0]; }

GameWithDummyAndFood::GameWithDummyAndFood()
	: game(&dungeon)
{
	game.level().map = Map(2, 2);
	dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));

	const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("armor").edible());
	const ItemType * spear = game.item_types.insert(ItemType::Builder("spear").sprite(2).damage(3).name("spear").edible());
	const ItemType * junk = game.item_types.insert(ItemType::Builder("junk").sprite(3).name("junk"));
	const ItemType * food = game.item_types.insert(ItemType::Builder("food").sprite(4).name("food").edible());
	const ItemType * medkit = game.item_types.insert(ItemType::Builder("medkit").sprite(4).name("medkit").edible().healing(5));
	const ItemType * megasphere = game.item_types.insert(ItemType::Builder("megasphere").sprite(4).name("megasphere").edible().healing(100));
	const ItemType * antidote = game.item_types.insert(ItemType::Builder("antidote").sprite(4).name("antidote").edible().antidote(5));
	const ItemType * full_flask = game.item_types.insert(ItemType::Builder("full_flask").sprite(7).name("water flask").edible());
	const ItemType * empty_flask = game.item_types.insert(ItemType::Builder("empty_flask").sprite(8).name("empty flask"));
	game.level().monsters.push_back(
			Monster::Builder(dummy_type).hp(90)
			.item(armor).item(spear).wear(0).wield(1)
			.item(junk).item(food).item(medkit).item(megasphere).item(antidote)
			.item(Item()).item(Item(full_flask, empty_flask)).item(Item::Builder(full_flask, empty_flask).make_empty())
			);
}
Monster & GameWithDummyAndFood::dummy() { return game.level().monsters[0]; }

GameWithDummyAndStairs::GameWithDummyAndStairs()
	: dungeon(Point(1, 1), Point(2, 2)), game(&dungeon)
{
	const MonsterType * dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").name("dummy"));
	const ObjectType * stairs_type = game.object_types.insert(ObjectType::Builder("stairs").name("stairs").transporting());
	game.item_types.insert(ItemType::Builder("yendor").name("Yendor").quest().sprite(1));
	game.level().map = Map(2, 2);
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
	game.level().objects.push_back(Object::Builder(stairs_type).pos(Point(1, 1)));
}
Monster & GameWithDummyAndStairs::dummy() { return game.level().monsters[0]; }
Object & GameWithDummyAndStairs::stairs() { return game.level().objects[0]; }

GameWithDummyAndObjects::GameWithDummyAndObjects()
	: game(&dungeon)
{
	game.level().map = Map(2, 2);
	floor_type = game.cell_types.insert(CellType::Builder("floor").name("floor").passable(true));
	dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
	stub_type = game.monster_types.insert(MonsterType::Builder("stub").name("stub"));
	game.object_types.insert(ObjectType::Builder("stub").name("stub"));
	opened_door = game.object_types.insert(ObjectType::Builder("opened_door").name("door").transparent().passable().openable().sprite(1));
	closed_door = game.object_types.insert(ObjectType::Builder("closed_door").name("door").openable().sprite(2));
	game.object_types.insert(ObjectType::Builder("pot").name("pot").containable());
	game.object_types.insert(ObjectType::Builder("well").name("well").drinkable());
	game.item_types.insert(ItemType::Builder("key").name("item").sprite(1));
	game.level().map.fill(floor_type);
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
}
Monster & GameWithDummyAndObjects::dummy() { return game.level().monsters[0]; }

GameWithDummyOnTrap::GameWithDummyOnTrap()
	: game(&dungeon)
{
	dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
	game.level().map = Map(2, 2);
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
	const ObjectType * trap_type = game.object_types.insert(ObjectType::Builder("trap").name("trap").triggerable());
	const ItemType * item = game.item_types.insert(ItemType::Builder("item").name("item").sprite(1));
	game.level().objects.push_back(Object::Builder(trap_type).pos(Point(1, 1)).item(item));
}
Monster & GameWithDummyOnTrap::dummy() { return game.level().monsters.front(); }

GameWithDummy::GameWithDummy()
	: game(&dungeon)
{
	game.level().map = Map(2, 2);
	game.cell_types.insert(CellType("floor"));
	dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
	player_type = game.monster_types.insert(MonsterType::Builder("player").max_hp(100).name("dummy").faction(Monster::PLAYER));

	game.level().map.fill(game.cell_types.get("floor"));
	const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("item"));
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)).item(armor));
	game.level().monsters.push_back(Monster::Builder(player_type).pos(Point(1, 1)).item(armor));
}
Monster & GameWithDummy::dummy() { return game.level().monsters.front(); }
Monster & GameWithDummy::player() { return game.level().monsters.back(); }

GameWithDummyAndKiller::GameWithDummyAndKiller()
	: game(&dungeon)
{
	const MonsterType * dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
	const MonsterType * killer_type = game.monster_types.insert(MonsterType::Builder("killer").max_hp(100).name("killer"));
	const MonsterType * poisoner_type = game.monster_types.insert(MonsterType::Builder("poisoner").max_hp(100).name("poisoner").poisonous(true));
	game.level().map = Map(2, 2);
	const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("item"));
	game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)).item(armor));
	game.level().monsters.push_back(Monster::Builder(killer_type).pos(Point(0, 1)));
	game.level().monsters.push_back(Monster::Builder(poisoner_type).pos(Point(1, 0)));
}
Monster & GameWithDummyAndKiller::dummy() { return game.level().monsters[0]; }
Monster & GameWithDummyAndKiller::killer() { return game.level().monsters[1]; }
Monster & GameWithDummyAndKiller::poisoner() { return game.level().monsters[2]; }

Game2x2::Game2x2()
	: game(&dungeon)
{
	game.level() = Level(2, 2);
	game.cell_types.insert(CellType::Builder("floor").sprite(1).passable(true).transparent(true));
	monster_type = game.monster_types.insert(MonsterType::Builder("monster").sprite(3).faction(Monster::PLAYER));
	game.object_types.insert(ObjectType::Builder("stone").name("stone"));
	game.object_types.insert(ObjectType::Builder("passable").passable().sprite(2));
	game.object_types.insert(ObjectType::Builder("transparent").transparent());
	game.item_types.insert(ItemType::Builder("item").sprite(4));
	game.level().map.fill(game.cell_types.get("floor"));
}

LevelWithPath::LevelWithPath()
	: game(&dungeon)
{
	game.level() = Level(4, 4);
	const CellType * f = game.cell_types.insert(CellType::Builder("f").passable(true));
	const CellType * w = game.cell_types.insert(CellType::Builder("w").passable(false));
	const CellType * a[] = {
		f, w, f, f,
		w, f, f, w,
		f, w, w, f,
		f, w, f, w,
	};
	game.level().map.fill(a);
}

LevelForSeeing::LevelForSeeing()
	: game(&dungeon)
{
	game.level() = Level(3, 2);
	const CellType * f = game.cell_types.insert(CellType::Builder("f").sprite(1).passable(true).transparent(true));
	const CellType * w = game.cell_types.insert(CellType::Builder("w").sprite(2).passable(false).transparent(false));
	const CellType * a[] = {
		w, f, w,
		f, w, f,
	};
	game.level().map.fill(a);
	const MonsterType * player_type = game.monster_types.insert(MonsterType::Builder("player").faction(Monster::PLAYER).sight(3).sprite(100));
	game.level().monsters.push_back(Monster::Builder(player_type).pos(Point(2, 1)));
}




};

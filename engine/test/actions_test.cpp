#include "../actions.h"
#include "../game.h"
#include "../monsters.h"
#include "../cell.h"
#include "../format.h"
#include "../test.h"

SUITE(actions) {

struct GameWithDummy {
	Game game;
	const CellType * floor_type;
	const MonsterType * dummy_type;
	const MonsterType * stub_type;
	const ObjectType * opened_door;
	const ObjectType * closed_door;
	GameWithDummy() {
		game.level.map = Map(2, 2);
		floor_type = game.cell_types.insert(CellType::Builder("floor").name("floor").passable(true));
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		stub_type = game.monster_types.insert(MonsterType::Builder("stub").name("stub"));
		game.object_types.insert(ObjectType::Builder("stub").name("stub"));
		opened_door = game.object_types.insert(ObjectType::Builder("opened_door").name("door").transparent().passable().openable().sprite(1));
		closed_door = game.object_types.insert(ObjectType::Builder("closed_door").name("door").openable().sprite(2));
		game.object_types.insert(ObjectType::Builder("pot").name("pot").containable());
		game.object_types.insert(ObjectType::Builder("well").name("well").drinkable());
		game.item_types.insert(ItemType::Builder("key").name("item").sprite(1));
		game.level.map.fill(floor_type);
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummy, should_move_when_cell_is_empty)
{
	Move action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 0));
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_impassable_cell)
{
	game.cell_types.insert(CellType::Builder("floor").name("wall").passable(false));
	Move action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy bump into the wall.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_monster)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Move action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy bump into stub.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_impassable_object)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("stub")).pos(Point(1, 0)));
	Move action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy bump into stub.").result);
}

TEST_FIXTURE(GameWithDummy, should_move_into_opened_object)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(true));
	Move action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 0));
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_closed_object)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Move action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Door is closed.").result);
}


TEST_FIXTURE(GameWithDummy, should_not_drink_monsters)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Drink action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("It is stub. dummy is not a vampire to drink that.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_drink_containers)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)));
	Drink action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Unfortunately, pot is totally empty.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_drink_at_empty_cell)
{
	Drink action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("There is nothing to drink.").result);
}

TEST_FIXTURE(GameWithDummy, should_drink_fountains)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 0)));
	Drink action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy drink from well.").result);
}

TEST_FIXTURE(GameWithDummy, should_heal_from_fountains)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 0)));
	dummy().hp -= 5;
	Drink action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(dummy().hp, 96);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy drink from well. It helps a bit.").result);
}


TEST_FIXTURE(GameWithDummy, should_not_open_already_opened_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(true));
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Door is already opened.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_closed_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_locked_doors_without_a_key)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false).locked(true).lock_type(1));
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(game.level.objects[0].locked);
	ASSERT(!game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Door is locked.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_locked_doors_with_a_key)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false).locked(true).lock_type(1));
	dummy().inventory.set_item(1, Item::Builder(game.item_types.get("key")).key_type(1));
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(!game.level.objects[0].locked);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unlocked the door.")("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_empty_cell)
{
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("There is nothing to open there.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_containers_and_drop_items)
{
	Item item(game.item_types.get("key"));
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)).item(item));
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.level.items[0].type, item.type);
	ASSERT(game.level.objects[0].items.empty());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy took up a item from pot.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_empty_containers)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)));
	Open action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(game.level.items.empty());
	ASSERT(game.level.objects[0].items.empty());
	EQUAL(game.messages.messages, MakeVector<std::string>("Pot is empty.").result);
}


TEST_FIXTURE(GameWithDummy, should_close_opened_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(true));
	Close action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(!game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy closed the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_close_already_closed_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Close action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(!game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Door is already closed.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_close_empty_cell)
{
	Close action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("There is nothing to close there.").result);
}


TEST_FIXTURE(GameWithDummy, should_hit_impassable_cells_on_swing)
{
	game.cell_types.insert(CellType::Builder("wall").name("wall").passable(false));
	game.level.map.set_cell_type(Point(1, 0), game.cell_types.get("wall"));
	Swing action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy hit wall.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_closed_doors_on_swing)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Swing action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy swing at door.")("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_hit_monsters_on_swing)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Swing action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy hit stub for 0 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_swing_at_nothing_at_empty_cell)
{
	Swing action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy swing at nothing.").result);
}


struct GameWithDummyWieldingAndWearing {
	Game game;
	const MonsterType * dummy_type;
	const MonsterType * stub_type;
	GameWithDummyWieldingAndWearing() {
		game.level.map = Map(2, 3);
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

		game.level.map.fill(game.cell_types.get("floor"));
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 2)).item(Item(spear)).item(Item(armor)).wield(0).wear(1).item(jacket));
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_throw_if_wields_nothing)
{
	dummy().inventory.unwield();
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy have nothing to throw.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_monster_when_thrown)
{
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_from_monster_when_thrown)
{
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_opaque_cell_and_drop_item_before_it)
{
	game.cell_types.insert(CellType::Builder("wall").name("wall").transparent(false));
	game.level.map.set_cell_type(Point(1, 0), game.cell_types.get("wall"));
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hit wall.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_impassable_object_and_drop_item_before_it)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("door")).pos(Point(1, 0)));
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hit door.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_container_and_drop_item_in_it)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)));
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear falls into pot.").result);
	ASSERT(game.level.items.empty());
	ASSERT(!game.level.objects[0].items.empty());
	EQUAL(game.level.objects[0].items[0].type->name, "spear");
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_fountain_and_erase_item_forever)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 0)));
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear falls into well. Forever lost.").result);
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_monster_and_drop_item_under_it)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Fire action(Point(0, -1));
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hits stub.")("Dummy hit stub for 3 hp.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 0));
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_drop_if_nothing_to_drop)
{
	dummy().inventory.clear();
	Drop action(0);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy have nothing to drop.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_drop_items_only_in_range)
{
	Drop action(4);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_before_dropping)
{
	Drop action(0);
	action.commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy dropped spear on the floor.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_before_dropping)
{
	Drop action(1);
	action.commit(dummy(), game);
	ASSERT(!dummy().inventory.worn_item().valid());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy dropped armor on the floor.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_inventory_when_dropped)
{
	Drop action(0);
	action.commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_place_item_on_the_floor_when_dropped)
{
	Drop action(0);
	action.commit(dummy(), game);
	EQUAL(game.level.items.size(), 1);
	EQUAL(game.level.items[0].type->name, "spear");
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_grab_if_floor_is_empty)
{
	Grab action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Nothing here to pick up.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_grabbed_item_to_the_first_empty_slot)
{
	game.level.items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 2)));
	Grab action;
	action.commit(dummy(), game);
	EQUAL(dummy().inventory.get_item(3).type->name, "item");
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy picked up item from the floor.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_grabbed_item_from_map)
{
	game.level.items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 2)));
	Grab action;
	action.commit(dummy(), game);
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_notify_if_quest_item)
{
	game.level.items.push_back(Item::Builder(game.item_types.get("quest_item")).pos(Point(1, 2)));
	Grab action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy picked up item from the floor.")("Now bring it back to the surface!").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_grab_item_if_inventory_is_full)
{
	for(int i = 2; i < 26; ++i) {
		dummy().inventory.insert(Item(game.item_types.get("stub")));
	}
	game.level.items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 2)));
	Grab action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy carry too much items.").result);
}


struct GameWithDummyWithItems {
	Game game;
	const MonsterType * dummy_type;
	GameWithDummyWithItems() {
		game.level.map = Map(2, 3);
		game.cell_types.insert(CellType::Builder("floor").passable(true).transparent(true).name("floor"));
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("armor"));
		const ItemType * spear = game.item_types.insert(ItemType::Builder("spear").sprite(2).damage(3).name("spear"));
		const ItemType * pot = game.item_types.insert(ItemType::Builder("pot").sprite(1).name("pot"));

		game.level.map.fill(game.cell_types.get("floor"));
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 2)).item(spear).item(armor).item(pot).item(pot));
		dummy().inventory.take_item(2);
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummyWithItems, should_wield_any_item)
{
	Wield action(0);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy wields spear.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wield_invalid_slot)
{
	Wield action(10);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wield_empty_slot)
{
	Wield action(2);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_previous_item_before_wielding_new)
{
	ItemType sword = ItemType::Builder("sword").sprite(1).name("sword");
	dummy().inventory.set_item(2, Item(&sword));
	Wield action(2);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy wields sword.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_before_wielding_it)
{
	Wield action(1);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy takes off armor.")("Dummy wields armor.").result);
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_if_wielded)
{
	Unwield action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unwields spear.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_unwield_item_if_not_wielded)
{
	Unwield action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy is wielding nothing.").result);
}


TEST_FIXTURE(GameWithDummyWithItems, should_wear_any_item)
{
	Wear action(1);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy wear armor.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_invalid_slot)
{
	Wear action(10);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_empty_slot)
{
	Wear action(2);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_unwearable_item)
{
	Wear action(3);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Pot cannot be worn.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_previous_item_before_wearing_new)
{
	Wear action(2);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy wear jacket.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_before_wearing_it)
{
	dummy().inventory.wield(2);
	Wear action(2);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unwields jacket.")("Dummy takes off armor.")("Dummy wear jacket.").result);
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_if_worn)
{
	TakeOff action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy takes off armor.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_take_off_item_if_not_worn)
{
	TakeOff action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy is wearing nothing.").result);
}


struct GameWithDummyAndFood {
	Game game;
	const MonsterType * dummy_type;
	enum { ARMOR, SPEAR, JUNK, FOOD, MEDKIT, MEGASPHERE, ANTIDOTE, EMPTY, NONE };
	GameWithDummyAndFood() {
		game.level.map = Map(2, 2);
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));

		const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("armor").edible());
		const ItemType * spear = game.item_types.insert(ItemType::Builder("spear").sprite(2).damage(3).name("spear").edible());
		const ItemType * junk = game.item_types.insert(ItemType::Builder("junk").sprite(3).name("junk"));
		const ItemType * food = game.item_types.insert(ItemType::Builder("food").sprite(4).name("food").edible());
		const ItemType * medkit = game.item_types.insert(ItemType::Builder("medkit").sprite(4).name("medkit").edible().healing(5));
		const ItemType * megasphere = game.item_types.insert(ItemType::Builder("megasphere").sprite(4).name("megasphere").edible().healing(100));
		const ItemType * antidote = game.item_types.insert(ItemType::Builder("antidote").sprite(4).name("antidote").edible().antidote(5));
		game.level.monsters.push_back(
				Monster::Builder(dummy_type).hp(90)
				.item(armor).item(spear).wear(0).wield(1)
				.item(junk).item(food).item(medkit).item(megasphere).item(antidote)
				.item(Item())
				);
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_invalid_slot)
{
	Eat action(NONE);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_empty_slot)
{
	Eat action(EMPTY);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_not_edible_item)
{
	Eat action(JUNK);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Junk isn't edible.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_take_off_item_before_eating)
{
	Eat action(ARMOR);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy eats armor.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_unwield_item_before_eating)
{
	Eat action(SPEAR);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy eats spear.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_eat_items)
{
	Eat action(FOOD);
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy eats food.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_heal_when_eating_healing_item)
{
	Eat action(MEDKIT);
	action.commit(dummy(), game);
	EQUAL(dummy().hp, 95);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy eats medkit.")("Medkit heals dummy.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_heal_up_to_max_hp_when_eating_healing_item)
{
	Eat action(MEGASPHERE);
	action.commit(dummy(), game);
	EQUAL(dummy().hp, 100);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy eats megasphere.")("Megasphere heals dummy.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_cure_poisoning_when_eating_antidote)
{
	dummy().poisoning = 10;
	Eat action(ANTIDOTE);
	action.commit(dummy(), game);
	EQUAL(dummy().poisoning, 5);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy eats antidote.")("Antidote cures poisoning a little.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_cure_poisoning_to_the_end_when_eating_antidote)
{
	dummy().poisoning = 5;
	Eat action(ANTIDOTE);
	action.commit(dummy(), game);
	EQUAL(dummy().poisoning, 0);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy eats antidote.")("Antidote cures poisoning.").result);
}


class TestLevelGenerator : public LevelGenerator {
public:
	TestLevelGenerator(const Point & player_pos1, const Point & player_pos2)
		: generated(false), pos1(player_pos1), pos2(player_pos2) { }
	virtual void create_types(Game & game)
	{
		player_one = game.monster_types.insert(MonsterType::Builder("player_one").sprite(1).faction(Monster::PLAYER));
		player_two = game.monster_types.insert(MonsterType::Builder("player_two").sprite(2).faction(Monster::PLAYER));
	}
	virtual void generate(Level & level, int level_index)
	{
		generated = true;
		level = Level(4, 4);
		if(level_index == 1) {
			level.monsters.push_back(Monster::Builder(player_one).pos(pos1));
		} else {
			level.monsters.push_back(Monster::Builder(player_two).pos(pos2));
		}
	}
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
	const MonsterType * player_one;
	const MonsterType * player_two;
};

struct GameWithDummyAndStairs {
	TestLevelGenerator generator;
	Game game;
	GameWithDummyAndStairs()
		: generator(Point(1, 1), Point(2, 2)), game(&generator)
	{
		const MonsterType * dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").name("dummy"));
		const ObjectType * stairs_type = game.object_types.insert(ObjectType::Builder("stairs").name("stairs").transporting());
		game.item_types.insert(ItemType::Builder("yendor").name("Yendor").quest().sprite(1));
		game.level.map = Map(2, 2);
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
		game.level.objects.push_back(Object::Builder(stairs_type).pos(Point(1, 1)));
	}
	Monster & dummy() { return game.level.monsters[0]; }
	Object & stairs() { return game.level.objects[0]; }
};

TEST_FIXTURE(GameWithDummyAndStairs, should_go_up_on_upstairs)
{
	stairs().up_destination = 1;
	GoUp action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy goes up.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_send_to_quest_on_upstairs_to_the_surface)
{
	stairs().up_destination = -1;
	GoUp action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy must complete mission in order to go back to the surface.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_win_game_on_upstairs_when_have_quest_item)
{
	dummy().inventory.insert(Item(game.item_types.get("yendor")));
	stairs().up_destination = -1;
	GoUp action;
	action.commit(dummy(), game);
	EQUAL(game.state, Game::COMPLETED);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy have brought Yendor to the surface. Yay! Game if finished.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_generate_corresponding_level_when_going_up)
{
	stairs().up_destination = 1;
	GoUp action;
	action.commit(dummy(), game);
	ASSERT(generator.was_generated());
}


TEST_FIXTURE(GameWithDummyAndStairs, should_go_down_on_downstairs)
{
	stairs().down_destination = 1;
	GoDown action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy goes down.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_send_to_quest_on_downstairs_to_the_surface)
{
	stairs().down_destination = -1;
	GoDown action;
	action.commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy must complete mission in order to go back to the surface.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_win_game_on_downstairs_when_have_quest_item)
{
	dummy().inventory.insert(Item(game.item_types.get("yendor")));
	stairs().down_destination = -1;
	GoDown action;
	action.commit(dummy(), game);
	EQUAL(game.state, Game::COMPLETED);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy have brought Yendor to the surface. Yay! Game if finished.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_generate_corresponding_level_when_going_down)
{
	stairs().down_destination = 1;
	GoDown action;
	action.commit(dummy(), game);
	ASSERT(generator.was_generated());
}

}

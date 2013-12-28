#include "../actions.h"
#include "../game.h"
#include "../test.h"

namespace DirectedActionsFixture {

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

struct GameWithDummyWieldingAndWearing {
	Game game;
	const MonsterType * dummy_type;
	const MonsterType * stub_type;
	GameWithDummyWieldingAndWearing() {
		TRACE(1);
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
		const ItemType * full_flask = game.item_types.insert(ItemType::Builder("full_flask").sprite(1).name("water flask"));
		const ItemType * empty_flask = game.item_types.insert(ItemType::Builder("empty_flask").sprite(2).name("empty flask"));

		game.level.map.fill(game.cell_types.get("floor"));
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 2)).
				item(Item(spear)).item(Item(armor)).wield(0).wear(1).
				item(jacket).item(Item::Builder(full_flask, empty_flask).make_empty()));
		TRACE(dummy().inventory.size());
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

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


}

SUITE(move) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummy, should_move_when_cell_is_empty)
{
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 0));
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_impassable_cell)
{
	game.cell_types.insert(CellType::Builder("floor").name("wall").passable(false));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy bump into the wall.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_monster)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy bump into stub.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_impassable_object)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("stub")).pos(Point(1, 0)));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy bump into stub.").result);
}

TEST_FIXTURE(GameWithDummy, should_move_into_opened_object)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(true));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 0));
}

TEST_FIXTURE(GameWithDummy, should_not_move_into_closed_object)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages.messages, MakeVector<std::string>("Door is closed.").result);
}

}

SUITE(drink) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummy, should_not_drink_monsters)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	CATCH(Drink(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::CANNOT_DRINK);
	}
}

TEST_FIXTURE(GameWithDummy, should_not_drink_not_drinkable_objects)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)));
	CATCH(Drink(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::NOTHING_TO_DRINK);
	}
}

TEST_FIXTURE(GameWithDummy, should_not_drink_at_empty_cell)
{
	CATCH(Drink(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::NOTHING_TO_DRINK);
	}
}

TEST_FIXTURE(GameWithDummy, should_drink_fountains)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 0)));
	Drink(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy drink from well.").result);
}

TEST_FIXTURE(GameWithDummy, should_heal_from_fountains)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 0)));
	dummy().hp -= 5;
	Drink(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().hp, 96);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy drink from well. It helps a bit.").result);
}

}

SUITE(open) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummy, should_not_open_already_opened_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(true));
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::ALREADY_OPENED);
		EQUAL(e.subject.name, "door");
	}
	ASSERT(game.level.objects[0].opened());
}

TEST_FIXTURE(GameWithDummy, should_open_closed_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Open(Point(0, -1)).commit(dummy(), game);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_locked_doors_without_a_key)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false).locked(true).lock_type(1));
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::LOCKED);
		EQUAL(e.subject.name, "door");
	}
	ASSERT(game.level.objects[0].locked);
	ASSERT(!game.level.objects[0].opened());
}

TEST_FIXTURE(GameWithDummy, should_open_locked_doors_with_a_key)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false).locked(true).lock_type(1));
	dummy().inventory.set_item(1, Item::Builder(game.item_types.get("key")).key_type(1));
	Open(Point(0, -1)).commit(dummy(), game);
	ASSERT(!game.level.objects[0].locked);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy unlocked the door.")("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_empty_cell)
{
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::NOTHING_TO_OPEN);
	}
}

TEST_FIXTURE(GameWithDummy, should_open_containers_and_drop_items)
{
	Item item(game.item_types.get("key"));
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)).item(item));
	Open(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.level.items[0].type, item.type);
	ASSERT(game.level.objects[0].items.empty());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy took up a item from pot.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_empty_containers)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)));
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::HAS_NO_ITEMS);
		EQUAL(e.subject.name, "pot");
	}
	ASSERT(game.level.items.empty());
	ASSERT(game.level.objects[0].items.empty());
}

}

SUITE(close) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummy, should_close_opened_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(true));
	Close(Point(0, -1)).commit(dummy(), game);
	ASSERT(!game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy closed the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_close_already_closed_doors)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	CATCH(Close(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::ALREADY_CLOSED);
	}
	ASSERT(!game.level.objects[0].opened());
}

TEST_FIXTURE(GameWithDummy, should_not_close_empty_cell)
{
	CATCH(Close(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::NOTHING_TO_CLOSE);
	}
}

}

SUITE(swing) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummy, should_hit_impassable_cells_on_swing)
{
	game.cell_types.insert(CellType::Builder("wall").name("wall").passable(false));
	game.level.map.set_cell_type(Point(1, 0), game.cell_types.get("wall"));
	Swing(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy hit wall.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_closed_doors_on_swing)
{
	game.level.objects.push_back(Object::Builder(closed_door, opened_door).pos(Point(1, 0)).opened(false));
	Swing(Point(0, -1)).commit(dummy(), game);
	ASSERT(game.level.objects[0].opened());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy swing at door.")("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_hit_monsters_on_swing)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Swing(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy hit stub for 0 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_swing_at_nothing_at_empty_cell)
{
	Swing(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy swing at nothing.").result);
}

}

SUITE(fire) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_throw_if_wields_nothing)
{
	dummy().inventory.unwield();
	CATCH(Fire(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::NOTHING_TO_THROW);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_monster_when_thrown)
{
	Fire(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_from_monster_when_thrown)
{
	Fire(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_opaque_cell_and_drop_item_before_it)
{
	game.cell_types.insert(CellType::Builder("wall").name("wall").transparent(false));
	game.level.map.set_cell_type(Point(1, 0), game.cell_types.get("wall"));
	Fire(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hit wall.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_impassable_object_and_drop_item_before_it)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("door")).pos(Point(1, 0)));
	Fire(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hit door.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_container_and_drop_item_in_it)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("pot")).pos(Point(1, 0)));
	Fire(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear falls into pot.").result);
	ASSERT(game.level.items.empty());
	ASSERT(!game.level.objects[0].items.empty());
	EQUAL(game.level.objects[0].items[0].type->name, "spear");
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_fountain_and_erase_item_forever)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 0)));
	Fire(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear falls into well. Forever lost.").result);
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_monster_and_drop_item_under_it)
{
	game.level.monsters.push_back(Monster::Builder(stub_type).pos(Point(1, 0)));
	Fire(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hits stub.")("Dummy hit stub for 3 hp.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 0));
}

}

SUITE(put) {

using namespace DirectedActionsFixture;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_put_if_wields_nothing)
{
	dummy().inventory.unwield();
	CATCH(Put(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::NOTHING_TO_PUT);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_monster_when_put)
{
	Put(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_from_monster_when_put)
{
	Put(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_item_on_the_floor_if_passable)
{
	Put(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy dropped spear on the floor.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_refill_item_if_emptyable_and_object_is_drinkable)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 1)));
	dummy().inventory.wield(3);
	Put(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy refill empty flask.").result);
	ASSERT(dummy().inventory.has_item(3));
	ASSERT(dummy().inventory.get_item(3).is_full());
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_refill_already_full_item)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("well")).pos(Point(1, 1)));
	dummy().inventory.get_item(3).make_full();
	dummy().inventory.wield(3);
	CATCH(Put(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, Action::ALREADY_FULL);
	}
	ASSERT(dummy().inventory.has_item(3));
	ASSERT(dummy().inventory.get_item(3).is_full());
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_item_under_monster_if_target_is_impassable)
{
	game.cell_types.insert(CellType::Builder("wall").name("wall").transparent(false));
	game.level.map.set_cell_type(Point(1, 1), game.cell_types.get("wall"));
	Put(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy dropped spear on the floor.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 2));
}

}

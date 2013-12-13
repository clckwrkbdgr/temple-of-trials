#include "../game.h"
#include "../test.h"

std::string to_string(const Control & control)
{
	return format("Control({0}, dir={1}, slot={2})", control.control, control.direction, control.slot);
}

class TestLevelGenerator : public LevelGenerator {
public:
	TestLevelGenerator(const Point & player_pos1, const Point & player_pos2);
	virtual void generate(Level & level, int level_index);
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
};

TestLevelGenerator::TestLevelGenerator(const Point & player_pos1, const Point & player_pos2)
	: generated(false), pos1(player_pos1), pos2(player_pos2)
{
}

void TestLevelGenerator::generate(Level & level, int level_index)
{
	generated = true;
	level = Level(4, 4);
	if(level_index == 1) {
		level.monsters.push_back(Monster::Builder().sprite(1).faction(Monster::PLAYER).pos(pos1));
	} else {
		level.monsters.push_back(Monster::Builder().sprite(2).faction(Monster::PLAYER).pos(pos2));
	}
}

SUITE(game) {

struct GameWithLevels {
	TestLevelGenerator generator;
	Game game;
	GameWithLevels(): generator(Point(1, 1), Point(2, 2)), game(&generator) {}
};

TEST_FIXTURE(GameWithLevels, should_save_current_level_as_visited)
{
	game.generate(1);
	game.generate(2);
	EQUAL(game.saved_levels.count(1), (unsigned)1);
}

TEST_FIXTURE(GameWithLevels, should_restore_player_from_the_old_level_at_new_pos)
{
	game.generate(1);
	game.generate(2);
	EQUAL(game.level.get_player().pos, Point(2, 2));
}

TEST_FIXTURE(GameWithLevels, should_restore_previously_visited_level)
{
	game.generate(1);
	game.level.get_player().sprite = 3;
	game.generate(2);
	game.generate(1);
	EQUAL(game.level.get_player().sprite, 3);
}

TEST_FIXTURE(GameWithLevels, should_generated_newly_visited_level)
{
	game.generate(1);
	EQUAL(game.level.get_player().sprite, 1);
}

TEST_FIXTURE(GameWithLevels, should_end_turn_after_generation)
{
	game.generate(1);
	ASSERT(game.turn_ended);
}


TEST(game_should_start_with_empty_messages)
{
	Game game;
	ASSERT(game.messages.empty());
}

TEST(should_accept_only_non_empty_messages)
{
	Game game;
	game.message("");
	ASSERT(game.messages.empty());
}

TEST(should_add_messages)
{
	Game game;
	game.message("hello");
	EQUAL(game.messages.size(), (unsigned)1);
}

TEST(should_titlecase_messages)
{
	Game game;
	game.message("hello");
	EQUAL(game.messages.front(), "Hello");
}

struct GameWithDummyOnTrap {
	Game game;
	GameWithDummyOnTrap() {
		game.level.map = Map(2, 2);
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
		game.level.objects.push_back(Object::Builder().pos(Point(1, 1)).name("trap").triggerable().item(Item::Builder().sprite(1)));
	}
};

TEST_FIXTURE(GameWithDummyOnTrap, should_trigger_trap_if_trap_is_set)
{
	game.process_environment(game.level.monsters.front());
	ASSERT(game.level.objects.front().items.empty());
}

TEST_FIXTURE(GameWithDummyOnTrap, should_hurt_monster_if_trap_is_set)
{
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 99);
	EQUAL(game.messages, MakeVector<std::string>("Dummy trigger the trap.")("Dummy loses 1 hp.").result);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_leave_bolt_if_trap_is_set)
{
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.items.front().sprite, 1);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_not_hurt_monster_if_trap_is_triggered_already)
{
	game.level.objects.front().items.clear();
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 100);
	EQUAL(game.messages, MakeVector<std::string>("Trap is already triggered.").result);
}

struct GameWithDummy {
	Game game;
	GameWithDummy() {
		game.level.map = Map(2, 2);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
	}
};

TEST_FIXTURE(GameWithDummy, should_hurt_monster_if_cell_hurts)
{
	game.level.map.celltypes[0].hurts = true;
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 99);
	EQUAL(game.messages, MakeVector<std::string>("It hurts!")("Dummy loses 1 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_hurt_monster_is_poisoned)
{
	game.level.monsters.front().poisoning = 10;
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 99);
	EQUAL(game.messages, MakeVector<std::string>("Dummy is poisoned.")("Dummy loses 1 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_decrease_poisoning_each_turn)
{
	game.level.monsters.front().poisoning = 10;
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().poisoning, 9);
}


TEST_FIXTURE(GameWithDummy, should_drop_loot_if_monster_is_dead)
{
	game.die(game.level.monsters.front());
	EQUAL(game.level.items.front().pos, game.level.monsters.front().pos);
	EQUAL(game.messages, MakeVector<std::string>("Dummy drops item.").result);
}

TEST_FIXTURE(GameWithDummy, should_end_game_if_player_is_dead)
{
	game.level.monsters.front().faction = Monster::PLAYER;
	game.die(game.level.monsters.front());
	ASSERT(game.done);
	ASSERT(game.player_died);
	EQUAL(game.messages, MakeVector<std::string>("Dummy drops item.")("You died.").result);
}


TEST_FIXTURE(GameWithDummy, should_hurt_monster)
{
	game.hurt(game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 95);
	EQUAL(game.messages, MakeVector<std::string>("Dummy loses 5 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_hurt_if_damage_exceeds_defence)
{
	game.level.monsters.front().worn = 0;
	game.hurt(game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 98);
	EQUAL(game.messages, MakeVector<std::string>("Dummy loses 2 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_hurt_at_full_damage_if_piercing)
{
	game.level.monsters.front().worn = 0;
	game.hurt(game.level.monsters.front(), 5, true);
	EQUAL(game.level.monsters.front().hp, 95);
	EQUAL(game.messages, MakeVector<std::string>("Dummy loses 5 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_die_if_hurts_too_much)
{
	game.hurt(game.level.monsters.front(), 105);
	ASSERT(game.level.monsters.front().is_dead());
	EQUAL(game.messages, MakeVector<std::string>("Dummy loses 105 hp and dies.")("Dummy drops item.").result);
}


struct GameWithDummyAndKiller {
	Game game;
	GameWithDummyAndKiller() {
		game.level.map = Map(2, 2);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
		game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer"));
	}
};

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_monster)
{
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 95);
	EQUAL(game.messages, MakeVector<std::string>("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_if_damage_exceeds_defence)
{
	game.level.monsters.front().worn = 0;
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 98);
	EQUAL(game.messages, MakeVector<std::string>("Killer hit dummy for 2 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster)
{
	game.level.monsters.back().poisonous = true;
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().poisoning, 5);
	EQUAL(game.messages, MakeVector<std::string>("Killer poisons dummy.")("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster_no_more_than_some_max)
{
	game.level.monsters.front().poisoning =  3;
	game.level.monsters.back().poisonous = true;
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().poisoning, 5);
	EQUAL(game.messages, MakeVector<std::string>("Killer poisons dummy.")("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_die_if_hit_was_too_much)
{
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 105);
	ASSERT(game.level.monsters.front().is_dead());
	EQUAL(game.messages, MakeVector<std::string>("Killer hit dummy for 105 hp and kills it.")("Dummy drops item.").result);
}

}

SUITE(game_actions) {

struct GameWithDummy {
	Game game;
	GameWithDummy() {
		game.level.map = Map(2, 2);
		game.level.map.celltypes.front().passable = true;
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummy, should_move_on_smart_move_if_passable)
{
	game.smart_move(dummy(), Point(0, -1));
	EQUAL(dummy().pos, Point(1, 0));
	ASSERT(game.messages.empty());
}

TEST_FIXTURE(GameWithDummy, should_open_door_on_smart_move_if_exists)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).opened(false).name("door"));
	game.smart_move(dummy(), Point(0, -1));
	ASSERT(game.level.doors.front().opened);
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_plan_to_move_in_just_opened_door_on_smart)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).opened(false).name("door"));
	game.smart_move(dummy(), Point(0, -1));
	EQUAL_CONTAINERS(dummy().plan, MakeVector<Control>(Control(Control::MOVE, Point(0, -1))).result);
}

TEST_FIXTURE(GameWithDummy, should_open_container_on_smart_move_if_exists)
{
	Item apple = Item::Builder().sprite(1).name("apple");
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("pot").containable().item(apple));
	game.smart_move(dummy(), Point(0, -1));
	ASSERT(game.level.objects.front().items.empty());
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.level.items, MakeVector<Item>(Item::Builder().sprite(1).name("apple").pos(Point(1, 1))).result);
	EQUAL(game.messages, MakeVector<std::string>("Dummy took up a apple from pot.").result);
}

TEST_FIXTURE(GameWithDummy, should_drink_from_fountain_on_smart_move_if_exists)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("well").drinkable());
	game.smart_move(dummy(), Point(0, -1));
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy drink from well.").result);
}

TEST_FIXTURE(GameWithDummy, should_swing_at_monster_on_smart_move_if_exists)
{
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 0)).name("stub"));
	game.smart_move(dummy(), Point(0, -1));
	EQUAL(dummy().pos, Point(1, 1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy hit stub for 0 hp.").result);
}


TEST_FIXTURE(GameWithDummy, should_not_drink_monsters)
{
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 0)).name("stub"));
	game.drink(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("It is stub. dummy is not a vampire to drink that.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_drink_containers)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("pot").containable());
	game.drink(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Unfortunately, pot is totally empty.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_drink_at_empty_cell)
{
	game.drink(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("There is nothing to drink.").result);
}

TEST_FIXTURE(GameWithDummy, should_drink_fountains)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("well").drinkable());
	game.drink(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy drink from well.").result);
}

TEST_FIXTURE(GameWithDummy, should_heal_from_fountains)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("well").drinkable());
	dummy().hp -= 5;
	game.drink(dummy(), Point(0, -1));
	EQUAL(dummy().hp, 96);
	EQUAL(game.messages, MakeVector<std::string>("Dummy drink from well. It helps a bit.").result);
}


TEST_FIXTURE(GameWithDummy, should_not_open_already_opened_doors)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).name("door").opened(true));
	game.open(dummy(), Point(0, -1));
	ASSERT(game.level.doors[0].opened);
	EQUAL(game.messages, MakeVector<std::string>("Door is already opened.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_closed_doors)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).name("door").opened(false));
	game.open(dummy(), Point(0, -1));
	ASSERT(game.level.doors[0].opened);
	EQUAL(game.messages, MakeVector<std::string>("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_empty_cell)
{
	game.open(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("There is nothing to open there.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_containers_and_drop_items)
{
	Item item = Item::Builder().sprite(1).name("item");
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("pot").containable().item(item));
	game.open(dummy(), Point(0, -1));
	EQUAL(game.level.items[0].sprite, item.sprite);
	ASSERT(game.level.objects[0].items.empty());
	EQUAL(game.messages, MakeVector<std::string>("Dummy took up a item from pot.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_open_empty_containers)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("pot").containable());
	game.open(dummy(), Point(0, -1));
	ASSERT(game.level.items.empty());
	ASSERT(game.level.objects[0].items.empty());
	EQUAL(game.messages, MakeVector<std::string>("Pot is empty.").result);
}


TEST_FIXTURE(GameWithDummy, should_close_opened_doors)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).name("door").opened(true));
	game.close(dummy(), Point(0, -1));
	ASSERT(!game.level.doors[0].opened);
	EQUAL(game.messages, MakeVector<std::string>("Dummy closed the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_close_already_closed_doors)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).name("door").opened(false));
	game.close(dummy(), Point(0, -1));
	ASSERT(!game.level.doors[0].opened);
	EQUAL(game.messages, MakeVector<std::string>("Door is already closed.").result);
}

TEST_FIXTURE(GameWithDummy, should_not_close_empty_cell)
{
	game.close(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("There is nothing to close there.").result);
}


TEST_FIXTURE(GameWithDummy, should_hit_impassable_cells_on_swing)
{
	int wall = game.level.map.add_cell_type(CellType::Builder().name("wall").passable(false));
	game.level.map.set_cell_type(Point(1, 0), wall);
	game.swing(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy hit wall.").result);
}

TEST_FIXTURE(GameWithDummy, should_open_closed_doors_on_swing)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).name("door").opened(false));
	game.swing(dummy(), Point(0, -1));
	ASSERT(game.level.doors[0].opened);
	EQUAL(game.messages, MakeVector<std::string>("Dummy swing at door.")("Dummy opened the door.").result);
}

TEST_FIXTURE(GameWithDummy, should_hit_monsters_on_swing)
{
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 0)).name("stub"));
	game.swing(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy hit stub for 0 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_swing_at_nothing_at_empty_cell)
{
	game.swing(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy swing at nothing.").result);
}


struct GameWithDummyWieldingAndWearing {
	Game game;
	GameWithDummyWieldingAndWearing() {
		game.level.map = Map(2, 3);
		int floor = game.level.map.add_cell_type(CellType::Builder().passable(true).transparent(true).name("floor"));
		game.level.map.fill(floor);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("armor");
		Item spear = Item::Builder().sprite(2).damage(3).name("spear");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 2)).hp(100).name("dummy").item(spear).item(armor).wield(0).wear(1));
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_throw_if_wields_nothing)
{
	dummy().wielded = -1;
	game.fire(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy have nothing to throw.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_monster_when_thrown)
{
	game.fire(dummy(), Point(0, -1));
	ASSERT(!dummy().inventory[0]);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_from_monster_when_thrown)
{
	game.fire(dummy(), Point(0, -1));
	EQUAL(dummy().wielded, -1);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_opaque_cell_and_drop_item_before_it)
{
	int wall = game.level.map.add_cell_type(CellType::Builder().name("wall").transparent(false));
	game.level.map.set_cell_type(Point(1, 0), wall);
	game.fire(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hit wall.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_closed_door_and_drop_item_before_it)
{
	game.level.doors.push_back(Door::Builder().pos(Point(1, 0)).name("door").opened(false));
	game.fire(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hit door.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_container_and_drop_item_in_it)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("pot").containable());
	game.fire(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy throw spear.")("Spear falls into pot.").result);
	ASSERT(game.level.items.empty());
	ASSERT(!game.level.objects[0].items.empty());
	EQUAL(game.level.objects[0].items[0].name, "spear");
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_fountain_and_erase_item_forever)
{
	game.level.objects.push_back(Object::Builder().pos(Point(1, 0)).name("well").drinkable());
	game.fire(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy throw spear.")("Spear falls into well. Forever lost.").result);
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_monster_and_drop_item_under_it)
{
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 0)).name("stub").hp(100));
	game.fire(dummy(), Point(0, -1));
	EQUAL(game.messages, MakeVector<std::string>("Dummy throw spear.")("Spear hits stub.")("Dummy hit stub for 3 hp.").result);
	ASSERT(!game.level.items.empty());
	EQUAL(game.level.items[0].pos, Point(1, 0));
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_drop_if_nothing_to_drop)
{
	dummy().inventory.clear();
	game.drop(dummy(), 0);
	EQUAL(game.messages, MakeVector<std::string>("Dummy have nothing to drop.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_drop_items_only_in_range)
{
	game.drop(dummy(), 4);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_before_dropping)
{
	game.drop(dummy(), 0);
	EQUAL(dummy().wielded, -1);
	EQUAL(game.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy dropped spear on the floor.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_before_dropping)
{
	game.drop(dummy(), 1);
	EQUAL(dummy().worn, -1);
	EQUAL(game.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy dropped armor on the floor.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_inventory_when_dropped)
{
	game.drop(dummy(), 0);
	ASSERT(!dummy().inventory[0]);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_place_item_on_the_floor_when_dropped)
{
	game.drop(dummy(), 0);
	EQUAL(game.level.items.size(), 1);
	EQUAL(game.level.items[0].name, "spear");
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_grab_if_floor_is_empty)
{
	game.grab(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Nothing here to pick up.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_grabbed_item_to_the_first_empty_slot)
{
	game.level.items.push_back(Item::Builder().pos(Point(1, 2)).name("item").sprite(1));
	game.grab(dummy());
	EQUAL(dummy().inventory[2].name, "item");
	EQUAL(game.messages, MakeVector<std::string>("Dummy picked up item from the floor.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_grabbed_item_from_map)
{
	game.level.items.push_back(Item::Builder().pos(Point(1, 2)).name("item").sprite(1));
	game.grab(dummy());
	ASSERT(game.level.items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_notify_if_quest_item)
{
	game.level.items.push_back(Item::Builder().pos(Point(1, 2)).name("item").sprite(1).quest());
	game.grab(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy picked up item from the floor.")("Now bring it back to the surface!").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_grab_item_if_inventory_is_full)
{
	for(int i = 2; i < 26; ++i) {
		dummy().inventory.push_back(Item::Builder().name("stub").sprite(2));
	}
	game.level.items.push_back(Item::Builder().pos(Point(1, 2)).name("item").sprite(1));
	game.grab(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy carry too much items.").result);
}


struct GameWithDummyWithItems {
	Game game;
	GameWithDummyWithItems() {
		game.level.map = Map(2, 3);
		int floor = game.level.map.add_cell_type(CellType::Builder().passable(true).transparent(true).name("floor"));
		game.level.map.fill(floor);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("armor");
		Item spear = Item::Builder().sprite(2).damage(3).name("spear");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 2)).hp(100).name("dummy").item(spear).item(armor).item(Item()));
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummyWithItems, should_wield_any_item)
{
	game.wield(dummy(), 0);
	EQUAL(game.messages, MakeVector<std::string>("Dummy wields spear.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wield_invalid_slot)
{
	game.wield(dummy(), 3);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wield_empty_slot)
{
	game.wield(dummy(), 2);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_previous_item_before_wielding_new)
{
	dummy().inventory.push_back(Item::Builder().sprite(1).name("sword"));
	game.wield(dummy(), 2);
	EQUAL(game.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy wields sword.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_before_wielding_it)
{
	game.wield(dummy(), 1);
	EQUAL(game.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy takes off armor.")("Dummy wields armor.").result);
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_if_wielded)
{
	game.unwield(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy unwields spear.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_unwield_item_if_not_wielded)
{
	game.unwield(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy is wielding nothing.").result);
}


TEST_FIXTURE(GameWithDummyWithItems, should_wear_any_item)
{
	game.wear(dummy(), 1);
	EQUAL(game.messages, MakeVector<std::string>("Dummy wear armor.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_invalid_slot)
{
	game.wear(dummy(), 3);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_empty_slot)
{
	game.wear(dummy(), 2);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_unwearable_item)
{
	dummy().inventory[2].sprite = 1;
	dummy().inventory[2].name = "pot";
	game.wear(dummy(), 2);
	EQUAL(game.messages, MakeVector<std::string>("Pot cannot be worn.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_previous_item_before_wearing_new)
{
	dummy().inventory.push_back(Item::Builder().sprite(1).name("jacket").wearable());
	game.wear(dummy(), 2);
	EQUAL(game.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy wear jacket.").result);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_before_wearing_it)
{
	dummy().inventory[0].wearable = true;
	game.wear(dummy(), 0);
	EQUAL(game.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy unwields spear.")("Dummy wear spear.").result);
}


TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_if_worn)
{
	game.take_off(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy takes off armor.").result);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_take_off_item_if_not_worn)
{
	game.take_off(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy is wearing nothing.").result);
}


struct GameWithDummyAndFood {
	Game game;
	enum { ARMOR, SPEAR, JUNK, FOOD, MEDKIT, MEGASPHERE, ANTIDOTE, EMPTY, NONE };
	GameWithDummyAndFood() {
		game.level.map = Map(2, 2);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("armor").edible();
		Item spear = Item::Builder().sprite(2).damage(3).name("spear").edible();
		Item junk = Item::Builder().sprite(3).name("junk");
		Item food = Item::Builder().sprite(4).name("food").edible();
		Item medkit = Item::Builder().sprite(4).name("medkit").edible().healing(5);
		Item megasphere = Item::Builder().sprite(4).name("megasphere").edible().healing(100);
		Item antidote = Item::Builder().sprite(4).name("antidote").edible().antidote(5);
		Item empty;
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").wield(1).wear(0));
		dummy().inventory << armor << spear << junk << food << medkit << megasphere << antidote << empty;
		dummy().hp = 90;
	}
	Monster & dummy() { return game.level.monsters[0]; }
};

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_invalid_slot)
{
	game.eat(dummy(), NONE);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_empty_slot)
{
	game.eat(dummy(), EMPTY);
	EQUAL(game.messages, MakeVector<std::string>("No such object.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_not_edible_item)
{
	game.eat(dummy(), JUNK);
	EQUAL(game.messages, MakeVector<std::string>("Junk isn't edible.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_take_off_item_before_eating)
{
	game.eat(dummy(), ARMOR);
	EQUAL(game.messages, MakeVector<std::string>("Dummy takes off armor.")("Dummy eats armor.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_unwield_item_before_eating)
{
	game.eat(dummy(), SPEAR);
	EQUAL(game.messages, MakeVector<std::string>("Dummy unwields spear.")("Dummy eats spear.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_eat_items)
{
	game.eat(dummy(), FOOD);
	EQUAL(game.messages, MakeVector<std::string>("Dummy eats food.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_heal_when_eating_healing_item)
{
	game.eat(dummy(), MEDKIT);
	EQUAL(dummy().hp, 95);
	EQUAL(game.messages, MakeVector<std::string>("Dummy eats medkit.")("Medkit heals dummy.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_heal_up_to_max_hp_when_eating_healing_item)
{
	game.eat(dummy(), MEGASPHERE);
	EQUAL(dummy().hp, 100);
	EQUAL(game.messages, MakeVector<std::string>("Dummy eats megasphere.")("Megasphere heals dummy.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_cure_poisoning_when_eating_antidote)
{
	dummy().poisoning = 10;
	game.eat(dummy(), ANTIDOTE);
	EQUAL(dummy().poisoning, 5);
	EQUAL(game.messages, MakeVector<std::string>("Dummy eats antidote.")("Antidote cures poisoning a little.").result);
}

TEST_FIXTURE(GameWithDummyAndFood, should_cure_poisoning_to_the_end_when_eating_antidote)
{
	dummy().poisoning = 5;
	game.eat(dummy(), ANTIDOTE);
	EQUAL(dummy().poisoning, 0);
	EQUAL(game.messages, MakeVector<std::string>("Dummy eats antidote.")("Antidote cures poisoning.").result);
}


struct GameWithDummyAndStairs {
	TestLevelGenerator generator;
	Game game;
	GameWithDummyAndStairs()
		: generator(Point(1, 1), Point(2, 2)), game(&generator)
	{
		game.level.map = Map(2, 2);
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).name("dummy"));
		game.level.objects.push_back(Object::Builder().pos(Point(1, 1)).name("stairs").transporting());
	}
	Monster & dummy() { return game.level.monsters[0]; }
	Object & stairs() { return game.level.objects[0]; }
};

TEST_FIXTURE(GameWithDummyAndStairs, should_go_up_on_upstairs)
{
	stairs().up_destination = 1;
	game.go_up(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy goes up.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_send_to_quest_on_upstairs_to_the_surface)
{
	stairs().up_destination = -1;
	game.go_up(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy must complete mission in order to go back to the surface.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_win_game_on_upstairs_when_have_quest_item)
{
	dummy().inventory.push_back(Item::Builder().sprite(1).quest().name("Yendor"));
	stairs().up_destination = -1;
	game.go_up(dummy());
	ASSERT(game.done);
	ASSERT(game.completed);
	EQUAL(game.messages, MakeVector<std::string>("Dummy have brought Yendor to the surface. Yay! Game if finished.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_generate_corresponding_level_when_going_up)
{
	stairs().up_destination = 1;
	game.go_up(dummy());
	ASSERT(generator.was_generated());
}


TEST_FIXTURE(GameWithDummyAndStairs, should_go_down_on_downstairs)
{
	stairs().down_destination = 1;
	game.go_down(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy goes down.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_send_to_quest_on_downstairs_to_the_surface)
{
	stairs().down_destination = -1;
	game.go_down(dummy());
	EQUAL(game.messages, MakeVector<std::string>("Dummy must complete mission in order to go back to the surface.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_win_game_on_downstairs_when_have_quest_item)
{
	dummy().inventory.push_back(Item::Builder().sprite(1).quest().name("Yendor"));
	stairs().down_destination = -1;
	game.go_down(dummy());
	ASSERT(game.done);
	ASSERT(game.completed);
	EQUAL(game.messages, MakeVector<std::string>("Dummy have brought Yendor to the surface. Yay! Game if finished.").result);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_generate_corresponding_level_when_going_down)
{
	stairs().down_destination = 1;
	game.go_down(dummy());
	ASSERT(generator.was_generated());
}

}

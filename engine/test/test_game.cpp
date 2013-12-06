#include "../game.h"
#include "../test.h"

class TestLevelGenerator : public LevelGenerator {
public:
	TestLevelGenerator(const Point & player_pos1, const Point & player_pos2);
	virtual void generate(Level & level, int level_index);
private:
	Point pos1, pos2;
};

TestLevelGenerator::TestLevelGenerator(const Point & player_pos1, const Point & player_pos2)
	: pos1(player_pos1), pos2(player_pos2)
{
}

void TestLevelGenerator::generate(Level & level, int level_index)
{
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
	EQUAL(game.level.get_player().sprite, 1);
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


TEST(should_hurt_monster_if_cell_hurts)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.map.celltypes[0].hurts = true;
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 99);
	std::string expected_messages[] = {
		"It hurts!",
		"Dummy loses 1 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_trigger_trap_if_trap_is_set)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.traps.push_back(Trap::Builder().pos(Point(1, 1)).name("trap"));
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.process_environment(game.level.monsters.front());
	ASSERT(game.level.traps.front().triggered);
}

TEST(should_hurt_monster_if_trap_is_set)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.traps.push_back(Trap::Builder().pos(Point(1, 1)).name("trap"));
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 99);
	std::string expected_messages[] = {
		"Dummy trigger the trap.",
		"Dummy loses 1 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_leave_bolt_if_trap_is_set)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.traps.push_back(Trap::Builder().pos(Point(1, 1)).name("trap").bolt(Item::Builder().sprite(1)));
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.items.front().sprite, 1);
}

TEST(should_not_hurt_monster_if_trap_is_triggered_already)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.traps.push_back(Trap::Builder().pos(Point(1, 1)).name("trap"));
	game.level.traps.front().triggered = true;
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 100);
	std::string expected_messages[] = {
		"Trap is already triggered."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_hurt_monster_is_poisoned)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.level.monsters.front().poisoning = 10;
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().hp, 99);
	std::string expected_messages[] = {
		"Dummy is poisoned.",
		"Dummy loses 1 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_decrease_poisoning_each_turn)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.level.monsters.front().poisoning = 10;
	game.process_environment(game.level.monsters.front());
	EQUAL(game.level.monsters.front().poisoning, 9);
}


TEST(should_drop_loot_if_monster_is_dead)
{
	Game game;
	game.level.map = Map(2, 2);
	Item item = Item::Builder().sprite(1).name("item");
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(item));
	game.die(game.level.monsters.front());
	EQUAL(game.level.items.front().pos, game.level.monsters.front().pos);
	std::string expected_messages[] = {
		"Dummy drops item."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_end_game_if_player_is_dead)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").faction(Monster::PLAYER));
	game.die(game.level.monsters.front());
	ASSERT(game.done);
	ASSERT(game.player_died);
	std::string expected_messages[] = {
		"You died."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}


TEST(should_hurt_monster)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.hurt(game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 95);
	std::string expected_messages[] = {
		"Dummy loses 5 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_hurt_if_damage_exceeds_defence)
{
	Game game;
	game.level.map = Map(2, 2);
	Item armor = Item::Builder().sprite(1).wearable().defence(3);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
	game.level.monsters.front().worn = 0;
	game.hurt(game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 98);
	std::string expected_messages[] = {
		"Dummy loses 2 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_hurt_at_full_damage_if_piercing)
{
	Game game;
	game.level.map = Map(2, 2);
	Item armor = Item::Builder().sprite(1).wearable().defence(3);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
	game.level.monsters.front().worn = 0;
	game.hurt(game.level.monsters.front(), 5, true);
	EQUAL(game.level.monsters.front().hp, 95);
	std::string expected_messages[] = {
		"Dummy loses 5 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_die_if_hurts_too_much)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.hurt(game.level.monsters.front(), 105);
	ASSERT(game.level.monsters.front().is_dead());
	std::string expected_messages[] = {
		"Dummy loses 105 hp and dies."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}


TEST(should_hit_monster)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer"));
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 95);
	std::string expected_messages[] = {
		"Killer hit dummy for 5 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_hit_if_damage_exceeds_defence)
{
	Game game;
	game.level.map = Map(2, 2);
	Item armor = Item::Builder().sprite(1).wearable().defence(3);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
	game.level.monsters.front().worn = 0;
	game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer"));
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().hp, 98);
	std::string expected_messages[] = {
		"Killer hit dummy for 2 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_poison_monster)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer").poisonous(true));
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().poisoning, 5);
	std::string expected_messages[] = {
		"Killer poisons dummy.",
		"Killer hit dummy for 5 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_poison_monster_no_more_than_some_max)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.level.monsters.front().poisoning =  3;
	game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer").poisonous(true));
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 5);
	EQUAL(game.level.monsters.front().poisoning, 5);
	std::string expected_messages[] = {
		"Killer poisons dummy.",
		"Killer hit dummy for 5 hp."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

TEST(should_die_if_hit_was_too_much)
{
	Game game;
	game.level.map = Map(2, 2);
	game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
	game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer"));
	game.hit(game.level.monsters.back(), game.level.monsters.front(), 105);
	ASSERT(game.level.monsters.front().is_dead());
	std::string expected_messages[] = {
		"Killer hit dummy for 105 hp and kills it."
	};
	EQUAL(game.messages, make_vector(expected_messages));
}

}

SUITE(game_actions) {

TEST(should_move_on_smart_move_if_passable)
{
	FAIL("not implemented");
}

TEST(should_open_door_on_smart_move_if_exists)
{
	FAIL("not implemented");
}

TEST(should_open_container_on_smart_move_if_exists)
{
	FAIL("not implemented");
}

TEST(should_drink_from_fountain_on_smart_move_if_exists)
{
	FAIL("not implemented");
}

TEST(should_swing_at_monster_on_smart_move_if_exists)
{
	FAIL("not implemented");
}


TEST(should_not_drink_monsters)
{
	FAIL("not implemented");
}

TEST(should_not_drink_containers)
{
	FAIL("not implemented");
}

TEST(should_not_drink_at_empty_cell)
{
	FAIL("not implemented");
}

TEST(should_drink_fountains)
{
	FAIL("not implemented");
}

TEST(should_heal_from_fountains)
{
	FAIL("not implemented");
}


TEST(should_not_open_already_opened_doors)
{
	FAIL("not implemented");
}

TEST(should_open_closed_doors)
{
	FAIL("not implemented");
}

TEST(should_not_open_empty_cell)
{
	FAIL("not implemented");
}

TEST(should_open_containers_and_drop_items)
{
	FAIL("not implemented");
}

TEST(should_not_open_empty_containers)
{
	FAIL("not implemented");
}


TEST(should_close_opened_doors)
{
	FAIL("not implemented");
}

TEST(should_not_close_already_closed_doors)
{
	FAIL("not implemented");
}

TEST(should_not_close_empty_cell)
{
	FAIL("not implemented");
}


TEST(should_hit_impassable_cells_on_swing)
{
	FAIL("not implemented");
}

TEST(should_open_closed_doors_on_swing)
{
	FAIL("not implemented");
}

TEST(should_hit_monsters_on_swing)
{
	FAIL("not implemented");
}

TEST(should_swing_at_nothing_at_empty_cell)
{
	FAIL("not implemented");
}


TEST(should_not_throw_in_wields_nothing)
{
	FAIL("not implemented");
}

TEST(should_remove_item_from_monster_when_thrown)
{
	FAIL("not implemented");
}

TEST(should_unwield_item_from_monster_when_thrown)
{
	FAIL("not implemented");
}

TEST(should_hit_opaque_cell_and_drop_item_before_it)
{
	FAIL("not implemented");
}

TEST(should_hit_closed_door_and_drop_item_before_it)
{
	FAIL("not implemented");
}

TEST(should_hit_container_and_drop_item_in_it)
{
	FAIL("not implemented");
}

TEST(should_hit_fountain_and_erase_item_forever)
{
	FAIL("not implemented");
}

TEST(should_hit_monster_and_drop_item_under_it)
{
	FAIL("not implemented");
}


TEST(should_not_drop_if_nothing_to_drop)
{
	FAIL("not implemented");
}

TEST(should_drop_items_only_in_range)
{
	FAIL("not implemented");
}

TEST(should_unwield_item_before_dropping)
{
	FAIL("not implemented");
}

TEST(should_take_off_item_before_dropping)
{
	FAIL("not implemented");
}

TEST(should_remove_item_from_inventory_when_dropped)
{
	FAIL("not implemented");
}

TEST(should_place_item_on_the_floor_when_dropped)
{
	FAIL("not implemented");
}


TEST(should_not_grab_if_floor_is_empty)
{
	FAIL("not implemented");
}

TEST(should_put_grabbed_item_to_the_first_empty_slot)
{
	FAIL("not implemented");
}

TEST(should_remove_grabbed_item_from_map)
{
	FAIL("not implemented");
}

TEST(should_notify_if_quest_item)
{
	FAIL("not implemented");
}

TEST(should_not_grab_item_if_inventory_is_full)
{
	FAIL("not implemented");
}


TEST(should_wield_any_item)
{
	FAIL("not implemented");
}

TEST(should_not_wield_invalid_slot)
{
	FAIL("not implemented");
}

TEST(should_not_wield_empty_slot)
{
	FAIL("not implemented");
}

TEST(should_unwield_previous_item_before_wielding_new)
{
	FAIL("not implemented");
}

TEST(should_take_off_item_before_wielding_it)
{
	FAIL("not implemented");
}


TEST(should_unwield_item_if_wielded)
{
	FAIL("not implemented");
}

TEST(should_not_unwield_item_if_not_wielded)
{
	FAIL("not implemented");
}


TEST(should_wear_any_item)
{
	FAIL("not implemented");
}

TEST(should_not_wear_invalid_slot)
{
	FAIL("not implemented");
}

TEST(should_not_wear_empty_slot)
{
	FAIL("not implemented");
}

TEST(should_not_wear_unwearable_item)
{
	FAIL("not implemented");
}

TEST(should_take_off_previous_item_before_wearing_new)
{
	FAIL("not implemented");
}

TEST(should_unwield_item_before_wearing_it)
{
	FAIL("not implemented");
}


TEST(should_take_off_item_if_worn)
{
	FAIL("not implemented");
}

TEST(should_not_take_off_item_if_not_worn)
{
	FAIL("not implemented");
}


TEST(should_not_eat_invalid_slot)
{
	FAIL("not implemented");
}

TEST(should_not_eat_empty_slot)
{
	FAIL("not implemented");
}

TEST(should_not_eat_not_edible_item)
{
	FAIL("not implemented");
}

TEST(should_take_off_item_before_eating)
{
	FAIL("not implemented");
}

TEST(should_unwield_item_before_eating)
{
	FAIL("not implemented");
}

TEST(should_eat_items)
{
	FAIL("not implemented");
}

TEST(should_heal_when_eating_healing_item)
{
	FAIL("not implemented");
}

TEST(should_heal_up_to_max_hp_when_eating_healing_item)
{
	FAIL("not implemented");
}

TEST(should_cure_poisoning_when_eating_antidote)
{
	FAIL("not implemented");
}


TEST(should_go_up_on_upstairs)
{
	FAIL("not implemented");
}

TEST(should_send_to_quest_on_upstairs_to_the_surface)
{
	FAIL("not implemented");
}

TEST(should_win_game_on_upstairs_when_have_quest_item)
{
	FAIL("not implemented");
}

TEST(should_generate_corresponding_level_when_going_up)
{
	FAIL("not implemented");
}


TEST(should_go_down_on_downstairs)
{
	FAIL("not implemented");
}

TEST(should_send_to_quest_on_downstairs_to_the_surface)
{
	FAIL("not implemented");
}

TEST(should_win_game_on_downstairs_when_have_quest_item)
{
	FAIL("not implemented");
}

TEST(should_generate_corresponding_level_when_going_down)
{
	FAIL("not implemented");
}

}

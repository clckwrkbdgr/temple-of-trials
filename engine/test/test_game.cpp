#include "../game.h"
#include "../test.h"

SUITE(game) {

TEST(should_save_current_level_as_visited)
{
	FAIL("not implemented");
}

TEST(should_restore_player_from_the_old_level_at_new_pos)
{
	FAIL("not implemented");
}

TEST(should_restore_previously_visited_level)
{
	FAIL("not implemented");
}

TEST(should_generated_newly_visited_level)
{
	FAIL("not implemented");
}

TEST(should_end_turn_after_generation)
{
	FAIL("not implemented");
}


TEST(should_accept_only_non_empty_messages)
{
	FAIL("not implemented");
}

TEST(should_add_messages)
{
	FAIL("not implemented");
}

TEST(should_titlecase_messages)
{
	FAIL("not implemented");
}


TEST(should_hurt_monster_if_cell_hurts)
{
	FAIL("not implemented");
}

TEST(should_trigger_trap_if_trap_is_set)
{
	FAIL("not implemented");
}

TEST(should_hurt_monster_if_trap_is_set)
{
	FAIL("not implemented");
}

TEST(should_leave_bolt_if_trap_is_set)
{
	FAIL("not implemented");
}

TEST(should_not_hurt_monster_if_trap_is_triggered_already)
{
	FAIL("not implemented");
}

TEST(should_hurt_monster_is_poisoned)
{
	FAIL("not implemented");
}

TEST(should_decrease_poisoning_each_turn)
{
	FAIL("not implemented");
}


TEST(should_drop_loot_if_monster_is_dead)
{
	FAIL("not implemented");
}

TEST(should_end_game_if_player_is_dead)
{
	FAIL("not implemented");
}


TEST(should_hurt_monster)
{
	FAIL("not implemented");
}

TEST(should_hurt_if_damage_exceeds_defence)
{
	FAIL("not implemented");
}

TEST(should_hurt_at_full_damage_if_piercing)
{
	FAIL("not implemented");
}

TEST(should_die_if_hurts_too_much)
{
	FAIL("not implemented");
}


TEST(should_hit_monster)
{
	FAIL("not implemented");
}

TEST(should_hit_if_damage_exceeds_defence)
{
	FAIL("not implemented");
}

TEST(should_poison_monster)
{
	FAIL("not implemented");
}

TEST(should_dir_if_hit_was_too_much)
{
	FAIL("not implemented");
}


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

}

SUITE(game_actions) {

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

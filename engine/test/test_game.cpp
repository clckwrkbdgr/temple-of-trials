#include "../game.h"
#include "../test.h"


TEST(should_save_current_level_as_visited)
{
}

TEST(should_restore_player_from_the_old_level_at_new_pos)
{
}

TEST(should_restore_previously_visited_level)
{
}

TEST(should_generated_newly_visited_level)
{
}

TEST(should_end_turn_after_generation)
{
}


TEST(should_accept_only_non_empty_messages)
{
}

TEST(should_add_messages)
{
}

TEST(should_titlecase_messages)
{
}


TEST(should_hurt_monster_if_cell_hurts)
{
}

TEST(should_trigger_trap_if_trap_is_set)
{
}

TEST(should_hurt_monster_if_trap_is_set)
{
}

TEST(should_leave_bolt_if_trap_is_set)
{
}

TEST(should_not_hurt_monster_if_trap_is_triggered_already)
{
}

TEST(should_hurt_monster_is_poisoned)
{
}

TEST(should_decrease_poisoning_each_turn)
{
}


TEST(should_drop_loot_if_monster_is_dead)
{
}

TEST(should_end_game_if_player_is_dead)
{
}


TEST(should_hurt_monster)
{
}

TEST(should_hurt_if_damage_exceeds_defence)
{
}

TEST(should_hurt_at_full_damage_if_piercing)
{
}

TEST(should_die_if_hurts_too_much)
{
}


TEST(should_hit_monster)
{
}

TEST(should_hit_if_damage_exceeds_defence)
{
}

TEST(should_poison_monster)
{
}

TEST(should_dir_if_hit_was_too_much)
{
}


TEST(should_move_on_smart_move_if_passable)
{
}

TEST(should_open_door_on_smart_move_if_exists)
{
}

TEST(should_open_container_on_smart_move_if_exists)
{
}

TEST(should_drink_from_fountain_on_smart_move_if_exists)
{
}

TEST(should_swing_at_monster_on_smart_move_if_exists)
{
}


TEST(should_not_drink_monsters)
{
}

TEST(should_not_drink_containers)
{
}

TEST(should_not_drink_at_empty_cell)
{
}

TEST(should_drink_fountains)
{
}

TEST(should_heal_from_fountains)
{
}


TEST(should_not_open_already_opened_doors)
{
}

TEST(should_open_closed_doors)
{
}

TEST(should_not_open_empty_cell)
{
}

TEST(should_open_containers_and_drop_items)
{
}

TEST(should_not_open_empty_containers)
{
}


TEST(should_close_opened_doors)
{
}

TEST(should_not_close_already_closed_doors)
{
}

TEST(should_not_close_empty_cell)
{
}


TEST(should_hit_impassable_cells_on_swing)
{
}

TEST(should_open_closed_doors_on_swing)
{
}

TEST(should_hit_monsters_on_swing)
{
}

TEST(should_swing_at_nothing_at_empty_cell)
{
}


TEST(should_not_throw_in_wields_nothing)
{
}

TEST(should_remove_item_from_monster_when_thrown)
{
}

TEST(should_unwield_item_from_monster_when_thrown)
{
}

TEST(should_hit_opaque_cell_and_drop_item_before_it)
{
}

TEST(should_hit_closed_door_and_drop_item_before_it)
{
}

TEST(should_hit_container_and_drop_item_in_it)
{
}

TEST(should_hit_fountain_and_erase_item_forever)
{
}

TEST(should_hit_monster_and_drop_item_under_it)
{
}


TEST(should_not_drop_if_nothing_to_drop)
{
}

TEST(should_drop_items_only_in_range)
{
}

TEST(should_unwield_item_before_dropping)
{
}

TEST(should_take_off_item_before_dropping)
{
}

TEST(should_remove_item_from_inventory_when_dropped)
{
}

TEST(should_place_item_on_the_floor_when_dropped)
{
}


TEST(should_not_grab_if_floor_is_empty)
{
}

TEST(should_put_grabbed_item_to_the_first_empty_slot)
{
}

TEST(should_remove_grabbed_item_from_map)
{
}

TEST(should_notify_if_quest_item)
{
}

TEST(should_not_grab_item_if_inventory_is_full)
{
}


TEST(should_wield_any_item)
{
}

TEST(should_not_wield_invalid_slot)
{
}

TEST(should_not_wield_empty_slot)
{
}

TEST(should_unwield_previous_item_before_wielding_new)
{
}

TEST(should_take_off_item_before_wielding_it)
{
}


TEST(should_unwield_item_if_wielded)
{
}

TEST(should_not_unwield_item_if_not_wielded)
{
}


TEST(should_wear_any_item)
{
}

TEST(should_not_wear_invalid_slot)
{
}

TEST(should_not_wear_empty_slot)
{
}

TEST(should_not_wear_unwearable_item)
{
}

TEST(should_take_off_previous_item_before_wearing_new)
{
}

TEST(should_unwield_item_before_wearing_it)
{
}


TEST(should_take_off_item_if_worn)
{
}

TEST(should_not_take_off_item_if_not_worn)
{
}


TEST(should_not_eat_invalid_slot)
{
}

TEST(should_not_eat_empty_slot)
{
}

TEST(should_not_eat_not_edible_item)
{
}

TEST(should_take_off_item_before_eating)
{
}

TEST(should_unwield_item_before_eating)
{
}

TEST(should_eat_items)
{
}

TEST(should_heal_when_eating_healing_item)
{
}

TEST(should_heal_up_to_max_hp_when_eating_healing_item)
{
}

TEST(should_cure_poisoning_when_eating_antidote)
{
}


TEST(should_go_up_on_upstairs)
{
}

TEST(should_send_to_quest_on_upstairs_to_the_surface)
{
}

TEST(should_win_game_on_upstairs_when_have_quest_item)
{
}

TEST(should_generate_corresponding_level_when_going_up)
{
}


TEST(should_go_down_on_downstairs)
{
}

TEST(should_send_to_quest_on_downstairs_to_the_surface)
{
}

TEST(should_win_game_on_downstairs_when_have_quest_item)
{
}

TEST(should_generate_corresponding_level_when_going_down)
{
}


#pragma once
#include <vector>
#include <string>
class Object;
class Item;
class Monster;
class CellType;

struct Messages {
	bool log_messages;
	std::vector<std::string> messages;

	Messages();
	void message(const std::string & text);


	void already_closed(const Object & object);
	void already_opened(const Object & object);
	void bumps_into(const Monster & someone, const CellType & cell);
	void bumps_into(const Monster & someone, const Monster & monster);
	void bumps_into(const Monster & someone, const Object & object);
	void cannot_be_eaten(const Item & item);
	void cannot_be_worn(const Item & item);
	void cannot_drink(const Monster & someone, const Monster & other);
	void cannot_go_down(const Monster & someone);
	void cannot_go_up(const Monster & someone);
	void carries_too_much_items(const Monster & someone);
	void closed(const Monster & someone, const Object & object);
	void cures_poisoning_a_little(const Item & item);
	void cures_poisoning_fully(const Item & item);
	void drink_container(const Object & object);
	void drink_empty_container(const Object & object);
	void drinks(const Monster & someone, const Object & object);
	void drinks_and_heals(const Monster & someone, const Object & object);
	void drops(const Monster & someone, const Item & item, const CellType & cell);
	void drops(const Monster & someone, const Item & item);
	void eats(const Monster & someone, const Item & item);
	void falls(const Item & item, const Object & object);
	void falls_and_lost(const Item & item, const Object & object);
	void goes_down(const Monster & someone);
	void goes_up(const Monster & someone);
	void heals(const Item & item, const Monster & someone);
	void hits(const Monster & someone, const CellType & cell);
	void hits(const Monster & someone, const Object & object);
	void hits(const Monster & someone, const Monster & other);
	void hits(const Item & item, const CellType & cell);
	void hits(const Item & item, const Object & object);
	void hits(const Item & item, const Monster & monster);
	void hits(const Monster & someone, const Monster & other, int damage);
	void hits_and_kills(const Monster & someone, const Monster & other, int damage);
	void terrain_hurts();
	void hurts(const Monster & someone, int received_damage);
	void hurts_and_dies(const Monster & someone, int received_damage);
	void is_closed(const Object & object);
	void is_empty(const Object & object);
	void is_locked(const Object & object);
	void no_such_object_in_inventory();
	void nothing_to_close();
	void nothing_to_drink();
	void nothing_to_drop(const Monster & someone);
	void nothing_to_eat(const Monster & someone);
	void nothing_to_open();
	void nothing_to_pick_up();
	void nothing_to_throw(const Monster & someone);
	void nothing_to_wear(const Monster & someone);
	void nothing_to_wield(const Monster & someone);
	void opened(const Monster & someone, const Object & object);
	void picks_up(const Monster & someone, const Item & item, const CellType & cell);
	void player_died();
	void poisoned(const Monster & someone);
	void poisons(const Monster & someone, const Monster & other);
	void return_to_gates_with_item();
	void sending_to_quest(const Monster & someone);
	void swings_at_nothing(const Monster & someone);
	void swings_at_object(const Monster & someone, const Object & object);
	void takes_off(const Monster & someone, const Item & item);
	void throws(const Monster & someone, const Item & item);
	void tooks_up_from(const Monster & someone, const Item & item, const Object & object);
	void trap_is_dead(const Object & object);
	void triggers_trap(const Monster & someone, const Object & object);
	void unlocks(const Monster & someone, const Object & object);
	void unwields(const Monster & someone, const Item & item);
	void wears(const Monster & someone, const Item & item);
	void wears_nothing(const Monster & someone);
	void wields(const Monster & someone, const Item & item);
	void wields_nothing(const Monster & someone);
	void win_the_game(const Monster & someone, const Item & quest_item);
};

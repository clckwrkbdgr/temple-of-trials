#include "actions.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "cell.h"
#include "util.h"
#include "format.h"

void Drop::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_drop(someone));
	ACTION_ASSERT(someone.inventory.get_item(slot).valid(), game.messages.no_such_object_in_inventory());
	if(someone.inventory.wields(slot)) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.wears(slot)) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	Item item = someone.inventory.take_item(slot);
	item.pos = someone.pos;
	game.level.items.push_back(item);
	game.messages.drops(someone, item, game.cell_type_at(someone.pos));
}

void Grab::commit(Monster & someone, Game & game)
{
	std::vector<Item>::iterator item_index;
	Item item = find_at(game.level.items, someone.pos, &item_index);
	ACTION_ASSERT(item.valid(), game.messages.nothing_to_pick_up());
	unsigned slot = someone.inventory.insert(item);
	ACTION_ASSERT(slot != Inventory::NOTHING, game.messages.carries_too_much_items(someone));
	game.level.items.erase(item_index);
	game.messages.picks_up(someone, item, game.cell_type_at(someone.pos));
	if(item.type->quest) {
		game.messages.return_to_gates_with_item();
	}
}

void Wield::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_wield(someone));
	ACTION_ASSERT(someone.inventory.get_item(slot).valid(), game.messages.no_such_object_in_inventory());
	if(someone.inventory.wielded_item().valid()) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.wears(slot)) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	someone.inventory.wield(slot);
	game.messages.wields(someone, someone.inventory.wielded_item());
}

void Unwield::commit(Monster & someone, Game & game)
{
	const Item & item = someone.inventory.wielded_item();
	ACTION_ASSERT(item.valid(), game.messages.wields_nothing(someone));
	game.messages.unwields(someone, item);
	someone.inventory.unwield();
}

void Wear::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_wear(someone));
	const Item & item = someone.inventory.get_item(slot);
	ACTION_ASSERT(item.valid(), game.messages.no_such_object_in_inventory());
	ACTION_ASSERT(item.type->wearable, game.messages.cannot_be_worn(item));
	if(someone.inventory.wields(slot)) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.worn_item().valid()) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	someone.inventory.wear(slot);
	game.messages.wears(someone, item);
}

void TakeOff::commit(Monster & someone, Game & game)
{
	const Item & item = someone.inventory.worn_item();
	ACTION_ASSERT(item.valid(), game.messages.wears_nothing(someone));
	game.messages.takes_off(someone, item);
	someone.inventory.unwield();
}

void Eat::commit(Monster & someone, Game & game)
{
	ACTION_ASSERT(!someone.inventory.empty(), game.messages.nothing_to_eat(someone));
	Item & item = someone.inventory.get_item(slot);
	ACTION_ASSERT(item.valid(), game.messages.no_such_object_in_inventory());
	ACTION_ASSERT(item.type->edible, game.messages.cannot_be_eaten(item));
	if(someone.inventory.wears(slot)) {
		game.messages.takes_off(someone, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	if(!item.is_emptyable() && someone.inventory.wields(slot)) {
		game.messages.unwields(someone, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	game.messages.eats(someone, item);
	if(item.type->antidote > 0 && someone.poisoning > 0) {
		someone.poisoning -= item.type->antidote;
		someone.poisoning = std::max(0, someone.poisoning);
		if(someone.poisoning > 0) {
			game.messages.cures_poisoning_a_little(item);
		} else {
			game.messages.cures_poisoning_fully(item);
		}
	}
	if(item.type->healing > 0) {
		if(someone.heal_by(item.type->healing)) {
			game.messages.heals(item, someone);
		}
	}
	if(item.is_emptyable()) {
		game.messages.is_empty_now(item);
		item.make_empty();
	} else {
		someone.inventory.take_item(slot);
	}
}

void GoUp::commit(Monster & someone, Game & game)
{
    Object & object = find_at(game.level.objects, someone.pos);
	ACTION_ASSERT(object.valid() && object.type->transporting && object.up_destination, game.messages.cannot_go_up(someone));
	if(object.is_exit_up()) {
		const Item & quest_item = someone.inventory.quest_item();
		if(quest_item.valid()) {
			game.messages.win_the_game(someone, quest_item);
			game.state = Game::COMPLETED;
		} else {
			game.messages.sending_to_quest(someone);
		}
	} else {
		game.messages.goes_up(someone);
		game.generate(object.up_destination);
	}
}

void GoDown::commit(Monster & someone, Game & game)
{
    Object & object = find_at(game.level.objects, someone.pos);
	ACTION_ASSERT(object.valid() && object.type->transporting && object.down_destination, game.messages.cannot_go_down(someone));
	if(object.is_exit_down()) {
		const Item & quest_item = someone.inventory.quest_item();
		if(quest_item.valid()) {
			game.messages.win_the_game(someone, quest_item);
			game.state = Game::COMPLETED;
		} else {
			game.messages.sending_to_quest(someone);
		}
	} else {
		game.messages.goes_down(someone);
		game.generate(object.down_destination);
	}
}

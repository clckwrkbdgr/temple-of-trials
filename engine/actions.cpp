#include "actions.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "cell.h"
#include "util.h"
#include "format.h"

Action::Exception::Exception(unsigned exception_type, const Info & action_subject, const Info & action_object)
	: type(exception_type), subject(action_subject), object(action_object)
{
}

void Action::assert(bool expression, unsigned exception_type, const Info & subject, const Info & object)
{
	if(!expression) {
		throw Exception(exception_type, subject, object);
	}
}


void Drop::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), NOTHING_TO_DROP, someone);
	assert(someone.inventory.get_item(slot).valid(), NO_SUCH_ITEM, someone);
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
	assert(item.valid(), NOTHING_TO_GRAB, someone);
	unsigned slot = someone.inventory.insert(item);
	assert(slot != Inventory::NOTHING, NO_SPACE_LEFT, someone);
	game.level.items.erase(item_index);
	game.messages.picks_up(someone, item, game.cell_type_at(someone.pos));
	if(item.type->quest) {
		game.messages.return_to_gates_with_item();
	}
}

void Wield::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), NOTHING_TO_WIELD, someone);
	assert(someone.inventory.get_item(slot).valid(), NO_SUCH_ITEM, someone);
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
	assert(item.valid(), NOTHING_TO_UNWIELD, someone);
	game.messages.unwields(someone, item);
	someone.inventory.unwield();
}

void Wear::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), NOTHING_TO_WEAR, someone);
	const Item & item = someone.inventory.get_item(slot);
	assert(item.valid(), NO_SUCH_ITEM, someone);
	assert(item.type->wearable, CANNOT_WEAR, someone, item);
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
	assert(item.valid(), NOTHING_TO_TAKE_OFF, someone);
	game.messages.takes_off(someone, item);
	someone.inventory.unwield();
}

void Eat::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), NOTHING_TO_EAT, someone);
	Item & item = someone.inventory.get_item(slot);
	assert(item.valid(), NO_SUCH_ITEM, someone);
	assert(item.type->edible, CANNOT_EAT, someone, item);
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
	assert(object.valid() && object.type->transporting && object.up_destination, CANNOT_GO_UP, someone);
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
	assert(object.valid() && object.type->transporting && object.down_destination, CANNOT_GO_DOWN, someone);
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

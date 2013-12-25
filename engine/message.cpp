#include "message.h"
#include "objects.h"
#include "monsters.h"
#include "map.h"
#include "util.h"

Messages::Messages()
	: log_messages(false)
{
}

void Messages::message(const std::string & text)
{
	if(text.empty()) {
		return;
	}
	std::string titled_text = text;
	titled_text[0] = toupper(titled_text[0]);
	messages.push_back(titled_text);
	if(log_messages) {
		log("Message: " + titled_text);
	}
}

void Messages::terrain_hurts()
{
	message("It hurts!");
}

void Messages::return_to_gates_with_item()
{
	message("Now bring it back to the surface!");
}

void Messages::nothing_to_drink(const Object & object)
{
	if(object.containable) {
		if(object.items.empty()) {
			message(format("Unfortunately, {0} is totally empty.", object.name));
		} else {
			message(format("Unfortunately, {0} has no water left. But there is something else inside.", object.name));
		}
	} else {
		message("There is nothing to drink.");
	}
}

void Messages::player_died()
{
	message("You died.");
}

void Messages::closed(const Monster & someone, const Object & object)
{
	message(format("{0} closed the {1}.", someone.type->name, object.name));
}

void Messages::cures_poisoning_a_little(const Item & item)
{
	message(format("{0} cures poisoning a little.", item.name));
}

void Messages::cures_poisoning_fully(const Item & item)
{
	message(format("{0} cures poisoning.", item.name));
}

void Messages::drops(const Monster & someone, const Item & item, const CellType & cell)
{
	message(format("{0} dropped {1} on the {2}.", someone.type->name, item.name, cell.name));
}

void Messages::drops(const Monster & someone, const Item & item)
{
	message(format("{0} drops {1}.", someone.type->name, item.name));
}

void Messages::eats(const Monster & someone, const Item & item)
{
	message(format("{0} eats {1}.", someone.type->name, item.name));
}

void Messages::falls_and_lost(const Item & item, const Object & object)
{
	message(format("{0} falls into {1}. Forever lost.", item.name, object.name));
}

void Messages::falls(const Item & item, const Object & object)
{
	message(format("{0} falls into {1}.", item.name, object.name));
}

void Messages::goes_down(const Monster & someone)
{
	message(format("{0} goes down.", someone.type->name));
}

void Messages::goes_up(const Monster & someone)
{
	message(format("{0} goes up.", someone.type->name));
}

void Messages::win_the_game(const Monster & someone, const Item & quest_item)
{
	message(format("{0} have brought {1} to the surface. Yay! Game if finished.", someone.type->name, quest_item.name));
}

void Messages::heals(const Item & item, const Monster & someone)
{
	message(format("{0} heals {1}.", item.name, someone.type->name));
}

void Messages::hits(const Monster & someone, const Monster & other, int damage)
{
	if(other.is_dead()) {
		message(format("{0} hit {1} for {2} hp and kills it.", someone.type->name, other.type->name, damage));
	} else {
		message(format("{0} hit {1} for {2} hp.", someone.type->name, other.type->name, damage));
	}
}

void Messages::hits(const Item & item, const CellType & cell)
{
	message(format("{0} hit {1}.", item.name, cell.name));
}

void Messages::hits(const Monster & someone, const CellType & cell)
{
	message(format("{0} hit {1}.", someone.type->name, cell.name));
}

void Messages::hits(const Monster & someone, const Object & object)
{
	message(format("{0} hit {1}.", someone.type->name, object.name));
}

void Messages::hits(const Item & item, const Object & object)
{
	message(format("{0} hit {1}.", item.name, object.name));
}

void Messages::hits(const Monster & someone, const Monster & other)
{
	message(format("{0} hits {1}.", someone.type->name, other.type->name));
}

void Messages::hits(const Item & item, const Monster & other)
{
	message(format("{0} hits {1}.", item.name, other.type->name));
}

void Messages::trap_is_dead(const Object & object)
{
	message(format("{0} is already triggered.", object.name));
}

void Messages::poisoned(const Monster & someone)
{
	message(format("{0} is poisoned.", someone.type->name));
}

void Messages::hurts(const Monster & someone, int received_damage)
{
	if(someone.is_dead()) {
		message(format("{0} loses {1} hp and dies.", someone.type->name, received_damage));
	} else {
		message(format("{0} loses {1} hp.", someone.type->name, received_damage));
	}
}

void Messages::sending_to_quest(const Monster & someone)
{
	message(format("{0} must complete mission in order to go back to the surface.", someone.type->name));
}

void Messages::opened(const Monster & someone, const Object & object)
{
	message(format("{0} opened the {1}.", someone.type->name, object.name));
}

void Messages::picks_up(const Monster & someone, const Item & item, const CellType & cell)
{
	message(format("{0} picked up {1} from the {2}.", someone.type->name, item.name, cell.name));
}

void Messages::poisons(const Monster & someone, const Monster & other)
{
	message(format("{0} poisons {1}.", someone.type->name, other.type->name));
}

void Messages::swings_at_nothing(const Monster & someone)
{
	message(format("{0} swing at nothing.", someone.type->name));
}

void Messages::swings_at_object(const Monster & someone, const Object & object)
{
	message(format("{0} swing at {1}.", someone.type->name, object.name));
}

void Messages::takes_off(const Monster & someone, const Item & item)
{
	message(format("{0} takes off {1}.", someone.type->name, item.name));
}

void Messages::throws(const Monster & someone, const Item & item)
{
	message(format("{0} throw {1}.", someone.type->name, item.name));
}

void Messages::tooks_up_from(const Monster & someone, const Item & item, const Object & object)
{
	message(format("{0} took up a {1} from {2}.", someone.type->name, item.name, object.name));
}

void Messages::triggers_trap(const Monster & someone, const Object & object)
{
	message(format("{0} trigger the {1}.", someone.type->name, object.name));
}

void Messages::unlocks(const Monster & someone, const Object & object)
{
	message(format("{0} unlocked the {1}.", someone.type->name, object.name));
}

void Messages::unwields(const Monster & someone, const Item & item)
{
	message(format("{0} unwields {1}.", someone.type->name, item.name));
}

void Messages::wears(const Monster & someone, const Item & item)
{
	message(format("{0} wear {1}.", someone.type->name, item.name));
}

void Messages::wields(const Monster & someone, const Item & item)
{
	message(format("{0} wields {1}.", someone.type->name, someone.inventory.wielded_item().name));
}

void Messages::cannot_drink(const Monster & someone, const Monster & other)
{
	message(format("It is {1}. {0} is not a vampire to drink that.", someone.type->name, other.type->name));
}

void Messages::no_such_object_in_inventory()
{
	message("No such object.");
}

void Messages::nothing_to_pick_up()
{
	message("Nothing here to pick up.");
}

void Messages::nothing_to_close()
{
	message("There is nothing to close there.");
}

void Messages::nothing_to_open()
{
	message("There is nothing to open there.");
}

void Messages::bumps_into(const Monster & someone, const CellType & cell)
{
	message(format("{0} bump into the {1}.", someone.type->name, cell.name));
}

void Messages::bumps_into(const Monster & someone, const Monster & monster)
{
	message(format("{0} bump into {1}.", someone.type->name, monster.type->name));
}

void Messages::bumps_into(const Monster & someone, const Object & object)
{
	if(object.openable) {
		if(object.locked) {
			message(format("{0} is locked.", object.name));
			return;
		} else {
			message(format("{0} is closed.", object.name));
			return;
		}
	}
	message(format("{0} bump into {1}.", someone.type->name, object.name));
}

void Messages::cannot_be_worn(const Item & item)
{
	message(format("{0} cannot be worn.", item.name));
}

void Messages::cannot_go_down(const Monster & someone)
{
	message(format("{0} cannot go down from there.", someone.type->name));
}

void Messages::cannot_go_up(const Monster & someone)
{
	message(format("{0} cannot go up from there.", someone.type->name));
}

void Messages::carries_too_much_items(const Monster & someone)
{
	message(format("{0} carry too much items.", someone.type->name));
}

void Messages::drinks(const Monster & someone, const Object & object)
{
	if(someone.hp < someone.type->max_hp) {
		message(format("{0} drink from {1}. It helps a bit.", someone.type->name, object.name));
	} else {
		message(format("{0} drink from {1}.", someone.type->name, object.name));
	}
}

void Messages::nothing_to_drop(const Monster & someone)
{
	message(format("{0} have nothing to drop.", someone.type->name));
}

void Messages::nothing_to_eat(const Monster & someone)
{
	message(format("{0} have nothing to eat.", someone.type->name));
}

void Messages::nothing_to_throw(const Monster & someone)
{
	message(format("{0} have nothing to throw.", someone.type->name));
}

void Messages::nothing_to_wear(const Monster & someone)
{
	message(format("{0} have nothing to wear.", someone.type->name));
}

void Messages::nothing_to_wield(const Monster & someone)
{
	message(format("{0} have nothing to wield.", someone.type->name));
}

void Messages::already_closed(const Object & object)
{
	message(format("{0} is already closed.", object.name));
}

void Messages::already_opened(const Object & object)
{
	message(format("{0} is already opened.", object.name));
}

void Messages::is_closed(const Object & object)
{
	message(format("{0} is closed.", object.name));
}

void Messages::is_empty(const Object & object)
{
	message(format("{0} is empty.", object.name));
}

void Messages::is_locked(const Object & object)
{
	message(format("{0} is locked.", object.name));
}

void Messages::wears_nothing(const Monster & someone)
{
	message(format("{0} is wearing nothing.", someone.type->name));
}

void Messages::wields_nothing(const Monster & someone)
{
	message(format("{0} is wielding nothing.", someone.type->name));
}

void Messages::cannot_be_eaten(const Item & item)
{
	message(format("{0} isn't edible.", item.name));
}


#include "message.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "map.h"
#include "log.h"
#include "format.h"
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

void Messages::message(const Action::Exception & e)
{
	switch(e.type) {
		case Action::ALREADY_CLOSED: message(format("{0} is already closed.", e.subject.name)); break;
		case Action::ALREADY_FULL: message(format("{0} is already full.", e.subject.name)); break;
		case Action::ALREADY_OPENED: message(format("{0} is already opened.", e.subject.name)); break;
		case Action::CANNOT_DRINK: message(format("{0} cannot drink {1}", e.subject.name, e.object.name)); break;
		case Action::CANNOT_EAT: message(format("{0} is not edible.", e.object.name)); break;
		case Action::CANNOT_GO_DOWN: message(format("{0} cannot go down there.", e.subject.name)); break;
		case Action::CANNOT_GO_UP: message(format("{0} cannot go up there.", e.subject.name)); break;
		case Action::CANNOT_WEAR: message(format("{0} cannot wear {1}.", e.subject.name, e.object.name)); break;
		case Action::LOCKED: message(format("{0} is locked.", e.subject.name)); break;
		case Action::NOTHING_TO_CLOSE: message("There is nothing to close there."); break;
		case Action::NOTHING_TO_DRINK: message("There is nothing to drink there."); break;
		case Action::NOTHING_TO_DROP: message(format("{0} has nothing to drop.", e.subject.name)); break;
		case Action::NOTHING_TO_EAT: message(format("{0} has nothing to eat.", e.subject.name)); break;
		case Action::NOTHING_TO_GRAB: message("There is nothing to grab there."); break;
		case Action::NOTHING_TO_OPEN: message("There is nothing to open there."); break;
		case Action::NOTHING_TO_TAKE_OFF: message(format("{0} have nothing to take off.", e.subject.name)); break;
		case Action::NOTHING_TO_UNWIELD: message(format("{0} have nothing to unwield.", e.subject.name)); break;
		case Action::NOTHING_TO_WEAR: message(format("{0} has nothing to drop.", e.subject.name)); break;
		case Action::NOTHING_TO_WIELD: message(format("{0} has nothing to drop.", e.subject.name)); break;
		case Action::NOTHING_TO_PUT: message(format("{0} have nothing to put down.", e.subject.name)); break;
		case Action::NOTHING_TO_THROW: message(format("{0} have nothing to throw.", e.subject.name)); break;
		case Action::NO_SPACE_LEFT: message(format("{0} carries too much items.", e.subject.name)); break;
		case Action::NO_SUCH_ITEM: message("No such item."); break;
		case Action::HAS_NO_ITEMS: message(format("{0} is totally empty.", e.subject.name)); break;
		default:
			log("Unknown action exception was catched: #{0} from type <{1}> to type <{2}>", e.type, e.subject.id, e.object.id);
			throw e;
	}
}

void Messages::message(const GameEvent & e)
{
	switch(e.type) {
		case GameEvent::CURES_POISONING: message("{0} cures {1}.", e.actor.name, e.target.name); break;
		case GameEvent::HEALS: message("{0} heals {1}.", e.actor.name, e.target.name); break;
		case GameEvent::HURTS: message("{0} hurts {1}!", e.actor.name, e.target.name); break;
		case GameEvent::IS_HURT_BY_POISONING: message("Poisoning hurts {0}!", e.actor.name); break;
		case GameEvent::LOSES_HEALTH: message("{0} loses {1} hp.", e.actor.name, e.amount); break;
		case GameEvent::DIED: message("{0} died.", e.actor.name); break;
		case GameEvent::HITS: message("{0} hits {1}.", e.actor.name, e.target.name); break;
		case GameEvent::HITS_FOR_HEALTH: message("{0} hits {1} for {2} hp.", e.actor.name, e.target.name, e.amount); break;
		case GameEvent::BUMPS_INTO: message("{0} bumps into {1}.", e.actor.name, e.target.name); break;
		case GameEvent::POISONS: message("{0} poisons {1}.", e.actor.name, e.target.name); break;
		case GameEvent::SWINGS_AT_NOTHING: message("{0} swing at nothing.", e.actor.name); break;
		case GameEvent::OPENS: message("{0} opens {1}.", e.actor.name, e.target.name); break;
		case GameEvent::CLOSES: message("{0} closes {1}.", e.actor.name, e.target.name); break;
		case GameEvent::DRINKS: message("{0} drinks from {1}.", e.actor.name, e.target.name); break;
		case GameEvent::GOES_DOWN: message("{0} goes down.", e.actor.name); break;
		case GameEvent::GOES_UP: message("{0} goes up.", e.actor.name); break;
		case GameEvent::UNLOCKS: message("{0} unloks {1}.", e.actor.name, e.target.name); break;
		case GameEvent::TRAP_IS_OUT_OF_ITEMS: message("{0} is out of bolts.", e.actor.name); break;
		case GameEvent::TRIGGERS: message("{0} triggers {1}.", e.actor.name, e.target.name); break;
		case GameEvent::EATS: message("{0} eats {1}.", e.actor.name, e.target.name); break;
		case GameEvent::EMPTIES: message("{0} is emptied.", e.target.name); break;
		case GameEvent::REFILLS: message("{0} is refilled.", e.target.name); break;
		case GameEvent::TAKES_OFF: message("{0} takes off {1}.", e.actor.name, e.target.name); break;
		case GameEvent::THROWS: message("{0} throws {1}.", e.actor.name, e.target.name); break;
		case GameEvent::UNWIELDS: message("{0} unwields {1}.", e.actor.name, e.target.name); break;
		case GameEvent::WEARS: message("{0} wears {1}.", e.actor.name, e.target.name); break;
		case GameEvent::WIELDS: message("{0} wields {1}.", e.actor.name, e.target.name); break;
		case GameEvent::DROPS_AT: message("{0} drops {1} at {2}.", e.actor.name, e.target.name, e.help.name); break;
		case GameEvent::FALLS_INTO: message("{0} falls into {1}.", e.actor.name, e.target.name); break;
		case GameEvent::PICKS_UP_FROM: message("{0} picks up {1} from {2}.", e.actor.name, e.target.name, e.help.name); break;
		case GameEvent::TAKES_FROM: message("{0} takes {1} from {2}.", e.actor.name, e.target.name, e.help.name); break;
		case GameEvent::PICKED_UP_A_QUEST_ITEM: message("Now get this {0} to the Temple Gate!", e.target.name); break;
		case GameEvent::SHOULD_GET_QUEST_ITEM: message("{0} should find explosives first!", e.actor.name); break;
		case GameEvent::WINS_GAME_WITH: message("{0} successfully brought {1} to the Temple Gate!", e.actor.name, e.target.name); break;
		default: log("Unknown event type #{0} with actor <{1}> and target <{2}>", e.type, e.actor.id, e.target.id);

	}
}

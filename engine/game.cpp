#include "game.h"
#include "actions.h"
#include "monsters.h"
#include "format.h"
#include <map>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <cmath>

GameEvent::GameEvent(const Info & event_actor, EventType event_type, const Info & event_target, const Info & event_help)
	: type(event_type), amount(0), actor(event_actor), target(event_target), help(event_help)
{
}

GameEvent::GameEvent(const Info & event_actor, EventType event_type, int event_amount, const Info & event_target, const Info & event_help)
	: type(event_type), amount(event_amount), actor(event_actor), target(event_target), help(event_help)
{
}

std::string to_string(const GameEvent & e)
{
	return format("<{1}> do #{0} ({2}) on <{3}> with <{4}>", e.type, e.actor.id, e.amount, e.target.id, e.help.id);
}


Game::Game(Dungeon * game_dungeon)
	: dungeon(game_dungeon), state(PLAYING), turns(0)
{
	assert(dungeon);
	dungeon->create_types(*this);
}

void Game::create_new_game()
{
	dungeon->go_to_level(1);
}

Level & Game::level()
{
	return dungeon->level();
}

const Level & Game::level() const
{
	return dungeon->level();
}

void Game::run(ControllerFactory controller_factory)
{
	state = PLAYING;
	while(state == PLAYING) {
		foreach(Monster & monster, level().monsters) {
			if(monster.is_dead()) {
				continue;
			}
			Controller controller = controller_factory(monster.type->ai);
			if(!controller) {
				log(format("No controller found for AI #{0}!", monster.type->ai));
				continue;
			}
			level().invalidate_fov(monster);
			Action * action = controller(monster, *this);
			if(action) {
				try {
					action->commit(monster, *this);
				} catch(const Action::Exception & e) {
					messages.message(e);
				}
				delete action;
				action = 0;
			}
			if(state == TURN_ENDED) {
				break;
			}

			process_environment(monster);

			if(state != PLAYING) {
				break;
			}
		}
		level().erase_dead_monsters();
		++turns;
		if(state == TURN_ENDED) {
			state = PLAYING;
		}
	}
}

void Game::event(const GameEvent & e)
{
	events.push_back(e);
}

void Game::event(const Info & event_actor, GameEvent::EventType event_type, const Info & event_target, const Info & event_help)
{
	events.push_back(GameEvent(event_actor, event_type, event_target, event_help));
}

void Game::event(const Info & event_actor, GameEvent::EventType event_type, int event_amount, const Info & event_target, const Info & event_help)
{
	events.push_back(GameEvent(event_actor, event_type, event_amount, event_target, event_help));
}

void Game::events_to_messages()
{
	foreach(const GameEvent & e, events) {
		messages.message(e);
	}
	events.clear();
}

void Game::process_environment(Monster & someone)
{
	if(level().cell_type_at(someone.pos).hurts) {
		event(level().cell_type_at(someone.pos), GameEvent::HURTS, someone);
		hurt(someone, 1);
	}
	Object & object = find_at(level().objects, someone.pos);
	if(object.valid() && object.type->triggerable) {
		if(object.items.empty()) {
			event(object, GameEvent::TRAP_IS_OUT_OF_ITEMS);
		} else {
			event(someone, GameEvent::TRIGGERS, object);
			object.items.back().pos = object.pos;
			level().items.push_back(object.items.back());
			object.items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		event(someone, GameEvent::IS_HURT_BY_POISONING);
		--someone.poisoning;
		hurt(someone, 1, true);
	} else if(someone.poisoning < 0) {
		someone.poisoning = 0;
	}
}

void Game::die(Monster & someone)
{
	Item item;
	while((item = someone.inventory.take_first_item()).valid()) {
		item.pos = someone.pos;
		level().items.push_back(item);
		event(someone, GameEvent::DROPS_AT, item, level().cell_type_at(someone.pos));
	}
	event(someone, GameEvent::DIED);
	if(someone.type->faction == Monster::PLAYER) {
		state = PLAYER_DIED;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.inventory.worn_item().type->defence);
	someone.hp -= received_damage;
	event(someone, GameEvent::LOSES_HEALTH, received_damage);
	if(someone.is_dead()) {
		die(someone);
	}
}

void Game::hit(Item & item, Monster & other, int damage)
{
	int received_damage = damage - other.inventory.worn_item().type->defence;
	other.hp -= received_damage;
	event(item, GameEvent::HITS_FOR_HEALTH, received_damage, other);
	if(other.is_dead()) {
		die(other);
	}
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.inventory.worn_item().type->defence;
	other.hp -= received_damage;
	event(someone, GameEvent::HITS_FOR_HEALTH, received_damage, other);
	if(someone.type->poisonous) {
		event(someone, GameEvent::POISONS, other);
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	if(other.is_dead()) {
		die(other);
	}
}


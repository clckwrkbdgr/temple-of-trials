#include "game.h"
#include "actions.h"
#include <map>
#include <memory>
#include <cstdlib>
#include <cassert>
#include <cmath>

#define GAME_ASSERT(condition, text) \
	do { if(!(condition)) { message(text); return; } } while(0)

Control::Control(int control_value)
	: control(control_value), slot(-1)
{
}

Control::Control(int control_value, const Point & control_direction)
	: control(control_value), direction(control_direction), slot(-1)
{
}

Control::Control(int control_value, int control_slot)
	: control(control_value), slot(control_slot)
{
}


Game::Game(LevelGenerator * level_generator)
	: log_messages(false), current_level(0), generator(level_generator), state(PLAYING), turns(0)
{
}

void Game::run(ControllerFactory controller_factory)
{
	state = PLAYING;
	while(state == PLAYING) {
		foreach(Monster & monster, level.monsters) {
			if(monster.is_dead()) {
				continue;
			}
			Controller controller = controller_factory(monster.ai);
			if(!controller) {
				log(format("No controller found for AI #{0}!", monster.ai));
				continue;
			}
			level.invalidate_fov(monster);
			Control control = controller(monster, *this);
			Action * action = 0;
			switch(control.control) {
				case Control::SMART_MOVE: action = new SmartMove(control.direction); break;
				case Control::MOVE: action = new Move(control.direction); break;
				case Control::OPEN: action = new Open(control.direction); break;
				case Control::CLOSE: action = new Close(control.direction); break;
				case Control::SWING: action = new Swing(control.direction); break;
				case Control::FIRE: action = new Fire(control.direction); break;
				case Control::DRINK: action = new Drink(control.direction); break;
				case Control::GRAB: action = new Grab(); break;
				case Control::DROP: action = new Drop(control.slot); break;
				case Control::WIELD: action = new Wield(control.slot); break;
				case Control::UNWIELD: action = new Unwield(); break;
				case Control::WEAR: action = new Wear(control.slot); break;
				case Control::TAKE_OFF: action = new TakeOff(); break;
				case Control::EAT: action = new Eat(control.slot); break;
				case Control::GO_UP: action = new GoUp(); break;
				case Control::GO_DOWN: action = new GoDown(); break;
				case Control::WAIT: break;
				default: log("Unknown control: {0}", control.control); break;
			}
			if(action) {
				action->commit(monster, *this);
			}
			if(state == TURN_ENDED) {
				break;
			}

			process_environment(monster);

			if(state != PLAYING) {
				break;
			}
		}
		level.erase_dead_monsters();
		++turns;
		if(state == TURN_ENDED) {
			state = PLAYING;
		}
	}
}

void Game::generate(int level_index)
{
	if(current_level != 0) {
		saved_levels[current_level] = level;
	}

	Monster player = level.get_player();
	if(saved_levels.count(level_index) > 0) {
		level = saved_levels[level_index];
		saved_levels.erase(level_index);
	} else if(generator) {
		generator->generate(level, level_index);
	} else {
		return;
	}
	if(player) {
		player.pos = level.get_player().pos;
		level.get_player() = player;
	}
	current_level = level_index;
	state = TURN_ENDED;
}

void Game::message(std::string text)
{
	if(text.empty()) {
		return;
	}
	text[0] = toupper(text[0]);
	messages.push_back(text);
	if(log_messages) {
		log("Message: " + text);
	}
}

void Game::process_environment(Monster & someone)
{
	if(level.map.cell(someone.pos).hurts) {
		message("It hurts!");
		hurt(someone, 1);
	}
	Object & object = find_at(level.objects, someone.pos);
	if(object && object.triggerable) {
		if(object.items.empty()) {
			message(format("{0} is already triggered.", object.name));
		} else {
			message(format("{0} trigger the {1}.", someone.name, object.name));
			object.items.back().pos = object.pos;
			level.items.push_back(object.items.back());
			object.items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		message(format("{0} is poisoned.", someone.name));
		--someone.poisoning;
		hurt(someone, 1, true);
	} else if(someone.poisoning < 0) {
		someone.poisoning = 0;
	}
}

void Game::die(Monster & someone)
{
	foreach(Item & item, someone.inventory) {
		item.pos = someone.pos;
		level.items.push_back(item);
		message(format("{0} drops {1}.", someone.name, item.name));
	}
	someone.inventory.clear();
	if(someone.faction == Monster::PLAYER) {
		message("You died.");
		state = PLAYER_DIED;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.worn_item().defence);
	if(!someone.godmode) {
		someone.hp -= received_damage;
	}
	GAME_ASSERT(someone.is_dead(), format("{0} loses {1} hp.", someone.name, received_damage));
	message(format("{0} loses {1} hp and dies.", someone.name, received_damage));
	die(someone);
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.worn_item().defence;
	if(!other.godmode) {
		other.hp -= received_damage;
	}
	if(someone.poisonous) {
		message(format("{0} poisons {1}.", someone.name, other.name));
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	GAME_ASSERT(other.is_dead(), format("{0} hit {1} for {2} hp.", someone.name, other.name, received_damage));
	message(format("{0} hit {1} for {2} hp and kills it.", someone.name, other.name, received_damage));
	die(other);
}


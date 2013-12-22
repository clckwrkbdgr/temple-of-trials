#include "game.h"
#include "actions.h"
#include <map>
#include <memory>
#include <cstdlib>
#include <cassert>
#include <cmath>

Game::Game(LevelGenerator * level_generator)
	: current_level(0), generator(level_generator), state(PLAYING), turns(0)
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
			Action * action = controller(monster, *this);
			if(action) {
				action->commit(monster, *this);
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
	if(player.valid()) {
		player.pos = level.get_player().pos;
		level.get_player() = player;
	}
	current_level = level_index;
	state = TURN_ENDED;
}

void Game::process_environment(Monster & someone)
{
	if(level.map.cell(someone.pos).hurts) {
		messages.terrain_hurts();
		hurt(someone, 1);
	}
	Object & object = find_at(level.objects, someone.pos);
	if(object.valid() && object.triggerable) {
		if(object.items.empty()) {
			messages.trap_is_dead(object);
		} else {
			messages.triggers_trap(someone, object);
			object.items.back().pos = object.pos;
			level.items.push_back(object.items.back());
			object.items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		messages.poisoned(someone);
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
		level.items.push_back(item);
		messages.drops(someone, item);
	}
	if(someone.faction == Monster::PLAYER) {
		messages.player_died();
		state = PLAYER_DIED;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : someone.inventory.worn_item().defence);
	someone.hp -= received_damage;
	messages.hurts(someone, received_damage);
	if(someone.is_dead()) {
		die(someone);
	}
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - other.inventory.worn_item().defence;
	other.hp -= received_damage;
	if(someone.poisonous) {
		messages.poisons(someone, other);
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	messages.hits(someone, other, received_damage);
	if(other.is_dead()) {
		die(other);
	}
}


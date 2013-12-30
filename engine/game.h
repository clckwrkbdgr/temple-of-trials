#pragma once
#include "map.h"
#include "objects.h"
#include "monsters.h"
#include "level.h"
#include "message.h"
#include "info.h"
#include <map>
#include <list>
class Game;

typedef Action* (*Controller)(Monster&, Game&);
typedef Controller (*ControllerFactory)(int ai);

struct GameEvent {
	enum EventType {
		UNKNOWN,

		CURES_POISONING, HEALS, HURTS, IS_HURT_BY_POISONING, LOSES_HEALTH, DIED,
		HITS, HITS_FOR_HEALTH, BUMPS_INTO, POISONS, SWINGS_AT_NOTHING,
		OPENS, CLOSES, DRINKS, GOES_DOWN, GOES_UP, UNLOCKS,
		TRAP_IS_OUT_OF_ITEMS, TRIGGERS,
		EATS, EMPTIES, REFILLS, TAKES_OFF, THROWS, UNWIELDS, WEARS, WIELDS,
		DROPS_AT, FALLS_INTO, PICKS_UP_FROM, TAKES_FROM,
		PICKED_UP_A_QUEST_ITEM, SHOULD_GET_QUEST_ITEM, WINS_GAME_WITH,

		COUNT
	};

	EventType type;
	int amount;
	Info actor, target, help;
	GameEvent(const Info & event_actor, EventType event_type, const Info & event_target = Info(), const Info & event_help = Info());
	GameEvent(const Info & event_actor, EventType event_type, int event_amount, const Info & event_target = Info(), const Info & event_help = Info());
};
std::string to_string(const GameEvent & e);

struct Game {
	// TODO all level stuff compile in one Dungeon class.
	enum State { PLAYING, TURN_ENDED, SUSPENDED, PLAYER_DIED, COMPLETED };
	Dungeon * dungeon;
	State state;
	Messages messages;
	int turns;
	std::vector<GameEvent> events;

	TypeRegistry<Cell> cell_types;
	TypeRegistry<Monster> monster_types;
	TypeRegistry<Object> object_types;
	TypeRegistry<Item> item_types;

	Game(Dungeon * game_dungeon);
	void run(ControllerFactory controller_factory);

	void event(const GameEvent & e);
	void event(const Info & event_actor, GameEvent::EventType event_type, const Info & event_target = Info(), const Info & event_help = Info());
	void event(const Info & event_actor, GameEvent::EventType event_type, int event_amount, const Info & event_target = Info(), const Info & event_help = Info());
	void events_to_messages();

	Level & level();
	const Level & level() const;
	void generate(int level);

	void process_environment(Monster & someone);
	void die(Monster & someone);
	void hurt(Monster & someone, int damage, bool pierce_armour = false);
	void hit(Monster & someone, Monster & other, int damage);
	void hit(Item & item, Monster & other, int damage);
};

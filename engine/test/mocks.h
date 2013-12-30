#pragma once
#include "../level.h"
#include "../game.h"

namespace GameMocks {

struct DummyDungeon : public Dungeon {
	DummyDungeon();
	virtual ~DummyDungeon();
	virtual void generate(Level & /*level*/, int /*level_index*/);
	virtual void create_types(Game & /*game*/);
};

class TestDungeon : public Dungeon {
public:
	TestDungeon(const Point & player_pos1, const Point & player_pos2);
	virtual void create_types(Game & game);
	virtual void generate(Level & level, int level_index);
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
	MonsterType player_one;
	MonsterType player_two;
};


struct GameWithDummyWieldingAndWearing {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	const MonsterType * stub_type;
	GameWithDummyWieldingAndWearing();
	Monster & dummy();
};

struct GameWithDummyWithItems {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	GameWithDummyWithItems();
	Monster & dummy();
};

struct GameWithDummyAndFood {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	enum { ARMOR, SPEAR, JUNK, FOOD, MEDKIT, MEGASPHERE, ANTIDOTE, FULL_FLASK, EMPTY_FLASK, EMPTY, NONE };
	GameWithDummyAndFood();
	Monster & dummy();
};

struct GameWithDummyAndStairs {
	TestDungeon dungeon;
	Game game;
	GameWithDummyAndStairs();
	Monster & dummy();
	Object & stairs();
};

struct GameWithDummyAndObjects {
	DummyDungeon dungeon;
	Game game;
	const CellType * floor_type;
	const MonsterType * dummy_type;
	const MonsterType * stub_type;
	const ObjectType * opened_door;
	const ObjectType * closed_door;
	GameWithDummyAndObjects();
	Monster & dummy();
};

struct GameWithDummyOnTrap {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	GameWithDummyOnTrap();
	Monster & dummy();
};

struct GameWithDummy {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	const MonsterType * player_type;
	GameWithDummy();
	Monster & dummy();
	Monster & player();
};

struct GameWithDummyAndKiller {
	DummyDungeon dungeon;
	Game game;
	GameWithDummyAndKiller();
	Monster & dummy();
	Monster & killer();
	Monster & poisoner();
};

struct Game2x2 {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * monster_type;
	Game2x2();
};

struct GameWithLevels {
	TestDungeon dungeon;
	GameWithLevels(): dungeon(Point(1, 1), Point(2, 2)) {}
};

struct LevelWithPath {
	DummyDungeon dungeon;
	Game game;
	LevelWithPath();
};

struct LevelForSeeing {
	DummyDungeon dungeon;
	Game game;
	LevelForSeeing();
};


}

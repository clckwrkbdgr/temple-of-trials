#include "../game.h"
#include "../test.h"

SUITE(game) {

class TestLevelGenerator : public LevelGenerator {
public:
	TestLevelGenerator(const Point & player_pos1, const Point & player_pos2)
		: generated(false), pos1(player_pos1), pos2(player_pos2) { }
	virtual void generate(Level & level, int level_index)
	{
		generated = true;
		level = Level(4, 4);
		if(level_index == 1) {
			level.monsters.push_back(Monster::Builder().sprite(1).faction(Monster::PLAYER).pos(pos1));
		} else {
			level.monsters.push_back(Monster::Builder().sprite(2).faction(Monster::PLAYER).pos(pos2));
		}
	}
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
};

struct GameWithLevels {
	TestLevelGenerator generator;
	Game game;
	GameWithLevels(): generator(Point(1, 1), Point(2, 2)), game(&generator) {}
};

TEST_FIXTURE(GameWithLevels, should_save_current_level_as_visited)
{
	game.generate(1);
	game.generate(2);
	EQUAL(game.saved_levels.count(1), (unsigned)1);
}

TEST_FIXTURE(GameWithLevels, should_restore_player_from_the_old_level_at_new_pos)
{
	game.generate(1);
	game.generate(2);
	EQUAL(game.level.get_player().pos, Point(2, 2));
}

TEST_FIXTURE(GameWithLevels, should_restore_previously_visited_level)
{
	game.generate(1);
	game.level.get_player().sprite = 3;
	game.generate(2);
	game.generate(1);
	EQUAL(game.level.get_player().sprite, 3);
}

TEST_FIXTURE(GameWithLevels, should_generated_newly_visited_level)
{
	game.generate(1);
	EQUAL(game.level.get_player().sprite, 1);
}

TEST_FIXTURE(GameWithLevels, should_end_turn_after_generation)
{
	game.generate(1);
	EQUAL(game.state, Game::TURN_ENDED);
}


struct GameWithDummyOnTrap {
	Game game;
	GameWithDummyOnTrap() {
		game.level.map = Map(2, 2);
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy"));
		game.level.objects.push_back(Object::Builder().pos(Point(1, 1)).name("trap").triggerable().item(Item::Builder().sprite(1)));
	}
	Monster & dummy() { return game.level.monsters.front(); }
};

TEST_FIXTURE(GameWithDummyOnTrap, should_trigger_trap_if_trap_is_set)
{
	game.process_environment(dummy());
	ASSERT(game.level.objects.front().items.empty());
}

TEST_FIXTURE(GameWithDummyOnTrap, should_hurt_monster_if_trap_is_set)
{
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy trigger the trap.")("Dummy loses 1 hp.").result);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_leave_bolt_if_trap_is_set)
{
	game.process_environment(dummy());
	EQUAL(game.level.items.front().sprite, 1);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_not_hurt_monster_if_trap_is_triggered_already)
{
	game.level.objects.front().items.clear();
	game.process_environment(dummy());
	EQUAL(dummy().hp, 100);
	EQUAL(game.messages.messages, MakeVector<std::string>("Trap is already triggered.").result);
}

struct GameWithDummy {
	Game game;
	GameWithDummy() {
		game.level.map = Map(2, 2);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
	}
	Monster & dummy() { return game.level.monsters.front(); }
};

TEST_FIXTURE(GameWithDummy, should_hurt_monster_if_cell_hurts)
{
	game.level.map.celltypes[0].hurts = true;
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	EQUAL(game.messages.messages, MakeVector<std::string>("It hurts!")("Dummy loses 1 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_hurt_monster_is_poisoned)
{
	dummy().poisoning = 10;
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy is poisoned.")("Dummy loses 1 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_decrease_poisoning_each_turn)
{
	dummy().poisoning = 10;
	game.process_environment(dummy());
	EQUAL(dummy().poisoning, 9);
}


TEST_FIXTURE(GameWithDummy, should_drop_loot_if_monster_is_dead)
{
	game.die(dummy());
	EQUAL(game.level.items.front().pos, dummy().pos);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy drops item.").result);
}

TEST_FIXTURE(GameWithDummy, should_end_game_if_player_is_dead)
{
	dummy().faction = Monster::PLAYER;
	game.die(dummy());
	EQUAL(game.state, Game::PLAYER_DIED);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy drops item.")("You died.").result);
}


TEST_FIXTURE(GameWithDummy, should_hurt_monster)
{
	game.hurt(dummy(), 5);
	EQUAL(dummy().hp, 95);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy loses 5 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_hurt_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hurt(dummy(), 5);
	EQUAL(dummy().hp, 98);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy loses 2 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_hurt_at_full_damage_if_piercing)
{
	dummy().inventory.wear(0);
	game.hurt(dummy(), 5, true);
	EQUAL(dummy().hp, 95);
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy loses 5 hp.").result);
}

TEST_FIXTURE(GameWithDummy, should_die_if_hurts_too_much)
{
	game.hurt(dummy(), 105);
	ASSERT(dummy().is_dead());
	EQUAL(game.messages.messages, MakeVector<std::string>("Dummy loses 105 hp and dies.")("Dummy drops item.").result);
}


struct GameWithDummyAndKiller {
	Game game;
	GameWithDummyAndKiller() {
		game.level.map = Map(2, 2);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).name("dummy").item(armor));
		game.level.monsters.push_back(Monster::Builder().pos(Point(0, 1)).hp(100).name("killer"));
	}
	Monster & dummy() { return game.level.monsters.front(); }
};

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_monster)
{
	game.hit(game.level.monsters.back(), dummy(), 5);
	EQUAL(dummy().hp, 95);
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hit(game.level.monsters.back(), dummy(), 5);
	EQUAL(dummy().hp, 98);
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer hit dummy for 2 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster)
{
	game.level.monsters.back().poisonous = true;
	game.hit(game.level.monsters.back(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer poisons dummy.")("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster_no_more_than_some_max)
{
	dummy().poisoning =  3;
	game.level.monsters.back().poisonous = true;
	game.hit(game.level.monsters.back(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer poisons dummy.")("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_die_if_hit_was_too_much)
{
	game.hit(game.level.monsters.back(), dummy(), 105);
	ASSERT(dummy().is_dead());
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer hit dummy for 105 hp and kills it.")("Dummy drops item.").result);
}

}


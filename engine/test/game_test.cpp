#include "../game.h"
#include "../monsters.h"
#include "../cell.h"
#include "../format.h"
#include "../test.h"

SUITE(game) {

class TestDungeon : public Dungeon {
public:
	TestDungeon(const Point & player_pos1, const Point & player_pos2)
		: generated(false), pos1(player_pos1), pos2(player_pos2)
	{
		player_one = MonsterType::Builder("player_one").sprite(1).faction(Monster::PLAYER);
		player_two = MonsterType::Builder("player_two").sprite(2).faction(Monster::PLAYER);
	}
	virtual void create_types(Game &) { }
	virtual void generate(Level & level, int level_index)
	{
		generated = true;
		level = Level(4, 4);
		if(level_index == 1) {
			level.monsters.push_back(Monster::Builder(&player_one).pos(pos1));
		} else {
			level.monsters.push_back(Monster::Builder(&player_two).pos(pos2));
		}
	}
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
	MonsterType player_one;
	MonsterType player_two;
};

struct GameWithLevels {
	TestDungeon dungeon;
	GameWithLevels(): dungeon(Point(1, 1), Point(2, 2)) {}
};

TEST_FIXTURE(GameWithLevels, should_save_current_level_as_visited)
{
	dungeon.go_to_level(1);
	dungeon.go_to_level(2);
	EQUAL(dungeon.saved_levels.count(1), (unsigned)1);
}

TEST_FIXTURE(GameWithLevels, should_restore_player_from_the_old_level_at_new_pos)
{
	dungeon.go_to_level(1);
	dungeon.go_to_level(2);
	EQUAL(dungeon.level().get_player().pos, Point(2, 2));
}

TEST_FIXTURE(GameWithLevels, should_restore_previously_visited_level)
{
	dungeon.go_to_level(1);
	dungeon.level().get_player().hp = 3;
	dungeon.go_to_level(2);
	dungeon.go_to_level(1);
	EQUAL(dungeon.level().get_player().hp, 3);
}

TEST_FIXTURE(GameWithLevels, should_generated_newly_visited_level)
{
	dungeon.go_to_level(1);
	EQUAL(dungeon.level().get_player().type->sprite, 1);
}


struct DummyDungeon : public Dungeon {
	DummyDungeon() : Dungeon() {}
	virtual ~DummyDungeon() {}
	virtual void generate(Level & /*level*/, int /*level_index*/) {}
	virtual void create_types(Game & /*game*/) {}
};

struct GameWithDummyOnTrap {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	GameWithDummyOnTrap() : game(&dungeon) {
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		game.level().map = Map(2, 2);
		game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
		const ObjectType * trap_type = game.object_types.insert(ObjectType::Builder("trap").name("trap").triggerable());
		const ItemType * item = game.item_types.insert(ItemType::Builder("item").name("item").sprite(1));
		game.level().objects.push_back(Object::Builder(trap_type).pos(Point(1, 1)).item(item));
	}
	Monster & dummy() { return game.level().monsters.front(); }
};

TEST_FIXTURE(GameWithDummyOnTrap, should_trigger_trap_if_trap_is_set)
{
	game.process_environment(dummy());
	ASSERT(game.level().objects.front().items.empty());
}

TEST_FIXTURE(GameWithDummyOnTrap, should_hurt_monster_if_trap_is_set)
{
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TRIGGERS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_leave_bolt_if_trap_is_set)
{
	game.process_environment(dummy());
	EQUAL(game.level().items.front().type->sprite, 1);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_not_hurt_monster_if_trap_is_triggered_already)
{
	game.level().objects.front().items.clear();
	game.process_environment(dummy());
	EQUAL(dummy().hp, 100);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TRAP_IS_OUT_OF_ITEMS);
	} DONE(e);
}

struct GameWithDummy {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * dummy_type;
	const MonsterType * player_type;
	GameWithDummy() : game(&dungeon) {
		game.level().map = Map(2, 2);
		game.cell_types.insert(CellType("floor"));
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		player_type = game.monster_types.insert(MonsterType::Builder("player").max_hp(100).name("dummy").faction(Monster::PLAYER));

		game.level().map.fill(game.cell_types.get("floor"));
		const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("item"));
		game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)).item(armor));
		game.level().monsters.push_back(Monster::Builder(player_type).pos(Point(1, 1)).item(armor));
	}
	Monster & dummy() { return game.level().monsters.front(); }
	Monster & player() { return game.level().monsters.back(); }
};

TEST_FIXTURE(GameWithDummy, should_hurt_monster_if_cell_hurts)
{
	game.cell_types.insert(CellType::Builder("floor").hurts(true));
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HURTS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_monster_is_poisoned)
{
	dummy().poisoning = 10;
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::IS_HURT_BY_POISONING);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
	} DONE(e);
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
	EQUAL(game.level().items.front().pos, dummy().pos);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DIED);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_end_game_if_player_is_dead)
{
	game.die(player());
	EQUAL(game.state, Game::PLAYER_DIED);
}


TEST_FIXTURE(GameWithDummy, should_hurt_monster)
{
	game.hurt(dummy(), 5);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 5);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hurt(dummy(), 5);
	EQUAL(dummy().hp, 98);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 2);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_at_full_damage_if_piercing)
{
	dummy().inventory.wear(0);
	game.hurt(dummy(), 5, true);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 5);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_die_if_hurts_too_much)
{
	game.hurt(dummy(), 105);
	ASSERT(dummy().is_dead());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 105);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DIED);
	} DONE(e);
}


struct GameWithDummyAndKiller {
	DummyDungeon dungeon;
	Game game;
	GameWithDummyAndKiller() : game(&dungeon) {
		const MonsterType * dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		const MonsterType * killer_type = game.monster_types.insert(MonsterType::Builder("killer").max_hp(100).name("killer"));
		const MonsterType * poisoner_type = game.monster_types.insert(MonsterType::Builder("poisoner").max_hp(100).name("poisoner").poisonous(true));
		game.level().map = Map(2, 2);
		const ItemType * armor = game.item_types.insert(ItemType::Builder("armor").sprite(1).wearable().defence(3).name("item"));
		game.level().monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)).item(armor));
		game.level().monsters.push_back(Monster::Builder(killer_type).pos(Point(0, 1)));
		game.level().monsters.push_back(Monster::Builder(poisoner_type).pos(Point(1, 0)));
	}
	Monster & dummy() { return game.level().monsters[0]; }
	Monster & killer() { return game.level().monsters[1]; }
	Monster & poisoner() { return game.level().monsters[2]; }
};

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_monster)
{
	game.hit(killer(), dummy(), 5);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 5);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hit(killer(), dummy(), 5);
	EQUAL(dummy().hp, 98);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 2);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster)
{
	game.hit(poisoner(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 5);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::POISONS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster_no_more_than_some_max)
{
	dummy().poisoning =  3;
	game.hit(poisoner(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 5);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::POISONS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_die_if_hit_was_too_much)
{
	game.hit(killer(), dummy(), 105);
	ASSERT(dummy().is_dead());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 105);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DIED);
	} DONE(e);
}

}


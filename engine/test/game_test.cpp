#include "../game.h"
#include "../monsters.h"
#include "../cell.h"
#include "../test.h"

SUITE(game) {

class TestLevelGenerator : public LevelGenerator {
public:
	TestLevelGenerator(const Point & player_pos1, const Point & player_pos2)
		: generated(false), pos1(player_pos1), pos2(player_pos2) { }
	virtual void create_types(Game & game)
	{
		player_one = game.monster_types.insert(MonsterType::Builder("player_one").sprite(1).faction(Monster::PLAYER));
		player_two = game.monster_types.insert(MonsterType::Builder("player_two").sprite(2).faction(Monster::PLAYER));
	}
	virtual void generate(Level & level, int level_index)
	{
		generated = true;
		level = Level(4, 4);
		if(level_index == 1) {
			level.monsters.push_back(Monster::Builder(player_one).pos(pos1));
		} else {
			level.monsters.push_back(Monster::Builder(player_two).pos(pos2));
		}
	}
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
	const MonsterType * player_one;
	const MonsterType * player_two;
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
	EQUAL(game.get_player().pos, Point(2, 2));
}

TEST_FIXTURE(GameWithLevels, should_restore_previously_visited_level)
{
	game.generate(1);
	game.get_player().hp = 3;
	game.generate(2);
	game.generate(1);
	EQUAL(game.get_player().hp, 3);
}

TEST_FIXTURE(GameWithLevels, should_generated_newly_visited_level)
{
	game.generate(1);
	EQUAL(game.get_player().type->sprite, 1);
}

TEST_FIXTURE(GameWithLevels, should_end_turn_after_generation)
{
	game.generate(1);
	EQUAL(game.state, Game::TURN_ENDED);
}


struct GameWithDummyOnTrap {
	Game game;
	const MonsterType * dummy_type;
	GameWithDummyOnTrap() {
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		game.level.map = Map(2, 2);
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)));
		const ObjectType * trap_type = game.object_types.insert(ObjectType::Builder("trap").name("trap").triggerable());
		game.level.objects.push_back(Object::Builder(trap_type).pos(Point(1, 1)).item(Item::Builder().sprite(1)));
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
	const MonsterType * dummy_type;
	const MonsterType * player_type;
	GameWithDummy() {
		game.level.map = Map(2, 2);
		game.cell_types.insert(CellType("floor"));
		dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		player_type = game.monster_types.insert(MonsterType::Builder("player").max_hp(100).name("dummy").faction(Monster::PLAYER));

		game.level.map.fill(game.cell_types.get("floor"));
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)).item(armor));
		game.level.monsters.push_back(Monster::Builder(player_type).pos(Point(1, 1)).item(armor));
	}
	Monster & dummy() { return game.level.monsters.front(); }
	Monster & player() { return game.level.monsters.back(); }
};

TEST_FIXTURE(GameWithDummy, should_hurt_monster_if_cell_hurts)
{
	game.cell_types.insert(CellType::Builder("floor").hurts(true));
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
	game.die(player());
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
		const MonsterType * dummy_type = game.monster_types.insert(MonsterType::Builder("dummy").max_hp(100).name("dummy"));
		const MonsterType * killer_type = game.monster_types.insert(MonsterType::Builder("killer").max_hp(100).name("killer"));
		const MonsterType * poisoner_type = game.monster_types.insert(MonsterType::Builder("poisoner").max_hp(100).name("poisoner").poisonous(true));
		game.level.map = Map(2, 2);
		Item armor = Item::Builder().sprite(1).wearable().defence(3).name("item");
		game.level.monsters.push_back(Monster::Builder(dummy_type).pos(Point(1, 1)).item(armor));
		game.level.monsters.push_back(Monster::Builder(killer_type).pos(Point(0, 1)));
		game.level.monsters.push_back(Monster::Builder(poisoner_type).pos(Point(1, 0)));
	}
	Monster & dummy() { return game.level.monsters[0]; }
	Monster & killer() { return game.level.monsters[1]; }
	Monster & poisoner() { return game.level.monsters[2]; }
};

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_monster)
{
	game.hit(killer(), dummy(), 5);
	EQUAL(dummy().hp, 95);
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hit(killer(), dummy(), 5);
	EQUAL(dummy().hp, 98);
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer hit dummy for 2 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster)
{
	game.hit(poisoner(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	EQUAL(game.messages.messages, MakeVector<std::string>("Poisoner poisons dummy.")("Poisoner hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster_no_more_than_some_max)
{
	dummy().poisoning =  3;
	game.hit(poisoner(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	EQUAL(game.messages.messages, MakeVector<std::string>("Poisoner poisons dummy.")("Poisoner hit dummy for 5 hp.").result);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_die_if_hit_was_too_much)
{
	game.hit(killer(), dummy(), 105);
	ASSERT(dummy().is_dead());
	EQUAL(game.messages.messages, MakeVector<std::string>("Killer hit dummy for 105 hp and kills it.")("Dummy drops item.").result);
}

}

SUITE(level) {

struct Game2x2 {
	Game game;
	const MonsterType * monster_type;
	Game2x2()
	{
		game.level = Level(2, 2);
		game.cell_types.insert(CellType::Builder("floor").sprite(1).passable(true).transparent(true));
		monster_type = game.monster_types.insert(MonsterType::Builder("monster").sprite(3).faction(Monster::PLAYER));
		game.object_types.insert(ObjectType::Builder("door").name("door").passable().openable().transparent());
		game.object_types.insert(ObjectType::Builder("passable").passable().sprite(2));
		game.object_types.insert(ObjectType::Builder("transparent").transparent());
		game.level.map.fill(game.cell_types.get("floor"));
	}
};

TEST_FIXTURE(Game2x2, impassable_cells_should_be_impassable)
{
	game.cell_types.insert(CellType::Builder("floor").passable(false));
	ASSERT(!game.is_passable(0, 0));
}

TEST_FIXTURE(Game2x2, monsters_should_be_impassable)
{
	game.level.monsters.push_back(Monster::Builder(monster_type).pos(Point(1, 1)));
	ASSERT(!game.is_passable(1, 1));
}

TEST_FIXTURE(Game2x2, items_should_be_passable)
{
	game.level.items.push_back(Item::Builder().pos(Point(1, 1)));
	ASSERT(game.is_passable(1, 1));
}

TEST_FIXTURE(Game2x2, impassable_objects_should_be_impassable)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("door")).pos(Point(1, 1)).opened(false));
	ASSERT(!game.is_passable(1, 1));
}

TEST_FIXTURE(Game2x2, passable_objects_should_be_passable)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	ASSERT(game.is_passable(1, 1));
}

TEST_FIXTURE(Game2x2, passable_cells_should_be_passable)
{
	ASSERT(game.is_passable(1, 1));
}


TEST_FIXTURE(Game2x2, opaque_cells_should_be_opaque)
{
	game.cell_types.insert(CellType::Builder("floor").transparent(false));
	ASSERT(!game.is_transparent(1, 1));
}

TEST_FIXTURE(Game2x2, opaque_objects_should_be_opaque)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("door")).pos(Point(1, 1)).opened(false));
	ASSERT(!game.is_transparent(1, 1));
}

TEST_FIXTURE(Game2x2, transparent_objects_should_be_transparent)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("transparent")).pos(Point(1, 1)));
	ASSERT(game.is_transparent(1, 1));
}

TEST_FIXTURE(Game2x2, monsters_should_be_transparent)
{
	game.level.monsters.push_back(Monster::Builder(monster_type).pos(Point(1, 1)));
	ASSERT(game.is_transparent(1, 1));
}

TEST_FIXTURE(Game2x2, items_should_be_transparent)
{
	game.level.items.push_back(Item::Builder().pos(Point(1, 1)));
	ASSERT(game.is_transparent(1, 1));
}

TEST_FIXTURE(Game2x2, transparent_cells_should_be_transparent)
{
	ASSERT(game.is_transparent(1, 1));
}


TEST_FIXTURE(Game2x2, monster_should_be_on_top_of_all)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	game.level.monsters.push_back(Monster::Builder(monster_type).pos(Point(1, 1)));
	game.level.items.push_back(Item::Builder().pos(Point(1, 1)).sprite(4));
	int sprite = game.get_sprite_at(Point(1, 1));
	EQUAL(sprite, 3);
}

TEST_FIXTURE(Game2x2, items_should_be_on_top_of_objects)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	game.level.items.push_back(Item::Builder().pos(Point(1, 1)).sprite(4));
	int sprite = game.get_sprite_at(Point(1, 1));
	EQUAL(sprite, 4);
}

TEST_FIXTURE(Game2x2, objects_should_be_below_everything)
{
	game.level.objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	int sprite = game.get_sprite_at(Point(1, 1));
	EQUAL(sprite, 2);
}


TEST_FIXTURE(Game2x2, should_get_player_from_monsters)
{
	game.level.monsters.push_back(Monster::Builder(monster_type));
	Monster & monster = game.get_player();
	EQUAL(monster.type->faction, Monster::PLAYER);
}

TEST_FIXTURE(Game2x2, should_get_player_from_const_monsters)
{
	game.level.monsters.push_back(Monster::Builder(monster_type));
	const Monster & monster = static_cast<const Game &>(game).get_player();
	EQUAL(monster.type->faction, Monster::PLAYER);
}


struct LevelWithPath {
	Game game;
	LevelWithPath()
	{
		game.level = Level(4, 4);
		const CellType * f = game.cell_types.insert(CellType::Builder("f").passable(true));
		const CellType * w = game.cell_types.insert(CellType::Builder("w").passable(false));
		const CellType * a[] = {
			f, w, f, f,
			w, f, f, w,
			f, w, w, f,
			f, w, f, w,
		};
		game.level.map.fill(a);
	}
};

TEST_FIXTURE(LevelWithPath, should_find_path_between_points)
{
	std::list<Point> path = game.find_path(Point(0, 3), Point(2, 3));
	EQUAL_CONTAINERS(path, MakeVector<Point>(Point(0, -1))(Point(1, -1))(Point(1,  0))(Point(1,  1))(Point(-1, 1)).result);
}

TEST_FIXTURE(LevelWithPath, should_find_path_between_close_points)
{
	std::list<Point> path = game.find_path(Point(0, 3), Point(0, 2));
	EQUAL_CONTAINERS(path, MakeVector<Point>(Point(0, -1)).result);
}

TEST_FIXTURE(LevelWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	std::list<Point> path = game.find_path(Point(0, 3), Point(0, 3));
	ASSERT(path.empty());
}

struct LevelForSeeing {
	Game game;
	LevelForSeeing()
	{
		game.level = Level(3, 2);
		const CellType * f = game.cell_types.insert(CellType::Builder("f").sprite(1).passable(true).transparent(true));
		const CellType * w = game.cell_types.insert(CellType::Builder("w").sprite(2).passable(false).transparent(false));
		const CellType * a[] = {
			w, f, w,
			f, w, f,
		};
		game.level.map.fill(a);
		const MonsterType * player_type = game.monster_types.insert(MonsterType::Builder("player").faction(Monster::PLAYER).sight(3).sprite(100));
		game.level.monsters.push_back(Monster::Builder(player_type).pos(Point(2, 1)));
	}
};

TEST_FIXTURE(LevelForSeeing, should_store_seen_sprites)
{
	game.invalidate_fov(game.get_player());
	EQUAL(game.level.map.cell(1, 0).seen_sprite, 1);
	EQUAL(game.level.map.cell(2, 0).seen_sprite, 2);
	EQUAL(game.level.map.cell(1, 1).seen_sprite, 2);
	EQUAL(game.level.map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_remember_sprite_instead_of_content)
{
	game.invalidate_fov(game.get_player());

	game.get_player().pos = Point(0, 1);
	game.invalidate_fov(game.get_player());
	EQUAL(game.level.map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_calculate_visible_area_within_sight_radius)
{
	game.invalidate_fov(game.get_player());
	EQUAL(game.level.map.cell(0, 0).visible, false);
	EQUAL(game.level.map.cell(0, 1).visible, false);
	EQUAL(game.level.map.cell(1, 0).visible, true);
	EQUAL(game.level.map.cell(2, 0).visible, true);
	EQUAL(game.level.map.cell(1, 1).visible, true);
	EQUAL(game.level.map.cell(2, 1).visible, true);
}

TEST_FIXTURE(LevelForSeeing, should_not_see_through_opaque_cells)
{
	game.invalidate_fov(game.get_player());
	EQUAL(game.level.map.cell(0, 1).visible, false);
}


TEST(should_erase_dead_monsters)
{
	Game game;
	game.level = Level(2, 2);
	const MonsterType * monster_alive = game.monster_types.insert(MonsterType::Builder("alive").max_hp(100).sprite(1));
	const MonsterType * monster_dead = game.monster_types.insert(MonsterType::Builder("dead").max_hp(100).sprite(2));
	game.level.monsters.push_back(Monster::Builder(monster_alive).pos(Point(1, 1)).hp(0));
	game.level.monsters.push_back(Monster::Builder(monster_dead).pos(Point(1, 1)));
	game.erase_dead_monsters();
	EQUAL(game.level.monsters.size(), (unsigned)1);
	EQUAL(game.level.monsters[0].type->sprite, 2);
}

}

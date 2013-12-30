#include "../level.h"
#include "../game.h"
#include "../test.h"

SUITE(dungeon) {

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
	EQUAL(dungeon.levels.count(2), (unsigned)1);
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

}

SUITE(level) {

struct DummyDungeon : public Dungeon {
	DummyDungeon() : Dungeon() {}
	virtual ~DummyDungeon() {}
	virtual void generate(Level & /*level*/, int /*level_index*/) {}
	virtual void create_types(Game & /*game*/) {}
};

struct Game2x2 {
	DummyDungeon dungeon;
	Game game;
	const MonsterType * monster_type;
	Game2x2()
		: game(&dungeon)
	{
		game.level() = Level(2, 2);
		game.cell_types.insert(CellType::Builder("floor").sprite(1).passable(true).transparent(true));
		monster_type = game.monster_types.insert(MonsterType::Builder("monster").sprite(3).faction(Monster::PLAYER));
		game.object_types.insert(ObjectType::Builder("stone").name("stone"));
		game.object_types.insert(ObjectType::Builder("passable").passable().sprite(2));
		game.object_types.insert(ObjectType::Builder("transparent").transparent());
		game.item_types.insert(ItemType::Builder("item").sprite(4));
		game.level().map.fill(game.cell_types.get("floor"));
	}
};

TEST_FIXTURE(Game2x2, impassable_cells_should_be_impassable)
{
	game.cell_types.insert(CellType::Builder("floor").passable(false));
	ASSERT(!game.level().get_info(0, 0).compiled().passable);
}

TEST_FIXTURE(Game2x2, monsters_should_be_impassable)
{
	game.level().monsters.push_back(Monster::Builder(monster_type).pos(Point(1, 1)));
	ASSERT(!game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, items_should_be_passable)
{
	game.level().items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 1)));
	ASSERT(game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, impassable_objects_should_be_impassable)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("stone")).pos(Point(1, 1)).opened(false));
	ASSERT(!game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, passable_objects_should_be_passable)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	ASSERT(game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, passable_cells_should_be_passable)
{
	ASSERT(game.level().get_info(1, 1).compiled().passable);
}


TEST_FIXTURE(Game2x2, opaque_cells_should_be_opaque)
{
	game.cell_types.insert(CellType::Builder("floor").transparent(false));
	ASSERT(!game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, opaque_objects_should_be_opaque)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("stone")).pos(Point(1, 1)).opened(false));
	ASSERT(!game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, transparent_objects_should_be_transparent)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("transparent")).pos(Point(1, 1)));
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, monsters_should_be_transparent)
{
	game.level().monsters.push_back(Monster::Builder(monster_type).pos(Point(1, 1)));
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, items_should_be_transparent)
{
	game.level().items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 1)));
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, transparent_cells_should_be_transparent)
{
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}


TEST_FIXTURE(Game2x2, monster_should_be_on_top_of_all)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	game.level().monsters.push_back(Monster::Builder(monster_type).pos(Point(1, 1)));
	game.level().items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 1)));
	int sprite = game.level().get_info(1, 1).compiled().sprite;
	EQUAL(sprite, 3);
}

TEST_FIXTURE(Game2x2, items_should_be_on_top_of_objects)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	game.level().items.push_back(Item::Builder(game.item_types.get("item")).pos(Point(1, 1)));
	int sprite = game.level().get_info(Point(1, 1)).compiled().sprite;
	EQUAL(sprite, 4);
}

TEST_FIXTURE(Game2x2, objects_should_be_below_everything)
{
	game.level().objects.push_back(Object::Builder(game.object_types.get("passable")).pos(Point(1, 1)));
	int sprite = game.level().get_info(Point(1, 1)).compiled().sprite;
	EQUAL(sprite, 2);
}


TEST_FIXTURE(Game2x2, should_get_player_from_monsters)
{
	game.level().monsters.push_back(Monster::Builder(monster_type));
	Monster & monster = game.level().get_player();
	EQUAL(monster.type->faction, Monster::PLAYER);
}

TEST_FIXTURE(Game2x2, should_get_player_from_const_monsters)
{
	game.level().monsters.push_back(Monster::Builder(monster_type));
	const Monster & monster = static_cast<const Game &>(game).level().get_player();
	EQUAL(monster.type->faction, Monster::PLAYER);
}


struct LevelWithPath {
	DummyDungeon dungeon;
	Game game;
	LevelWithPath()
		: game(&dungeon)
	{
		game.level() = Level(4, 4);
		const CellType * f = game.cell_types.insert(CellType::Builder("f").passable(true));
		const CellType * w = game.cell_types.insert(CellType::Builder("w").passable(false));
		const CellType * a[] = {
			f, w, f, f,
			w, f, f, w,
			f, w, w, f,
			f, w, f, w,
		};
		game.level().map.fill(a);
	}
};

TEST_FIXTURE(LevelWithPath, should_find_path_between_points)
{
	std::list<Point> path = game.level().find_path(Point(0, 3), Point(2, 3));
	EQUAL_CONTAINERS(path, MakeVector<Point>(Point(0, -1))(Point(1, -1))(Point(1,  0))(Point(1,  1))(Point(-1, 1)).result);
}

TEST_FIXTURE(LevelWithPath, should_find_path_between_close_points)
{
	std::list<Point> path = game.level().find_path(Point(0, 3), Point(0, 2));
	EQUAL_CONTAINERS(path, MakeVector<Point>(Point(0, -1)).result);
}

TEST_FIXTURE(LevelWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	std::list<Point> path = game.level().find_path(Point(0, 3), Point(0, 3));
	ASSERT(path.empty());
}

struct LevelForSeeing {
	DummyDungeon dungeon;
	Game game;
	LevelForSeeing()
		: game(&dungeon)
	{
		game.level() = Level(3, 2);
		const CellType * f = game.cell_types.insert(CellType::Builder("f").sprite(1).passable(true).transparent(true));
		const CellType * w = game.cell_types.insert(CellType::Builder("w").sprite(2).passable(false).transparent(false));
		const CellType * a[] = {
			w, f, w,
			f, w, f,
		};
		game.level().map.fill(a);
		const MonsterType * player_type = game.monster_types.insert(MonsterType::Builder("player").faction(Monster::PLAYER).sight(3).sprite(100));
		game.level().monsters.push_back(Monster::Builder(player_type).pos(Point(2, 1)));
	}
};

TEST_FIXTURE(LevelForSeeing, should_store_seen_sprites)
{
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(1, 0).seen_sprite, 1);
	EQUAL(game.level().map.cell(2, 0).seen_sprite, 2);
	EQUAL(game.level().map.cell(1, 1).seen_sprite, 2);
	EQUAL(game.level().map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_remember_sprite_instead_of_content)
{
	game.level().invalidate_fov(game.level().get_player());

	game.level().get_player().pos = Point(0, 1);
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_calculate_visible_area_within_sight_radius)
{
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(0, 0).visible, false);
	EQUAL(game.level().map.cell(0, 1).visible, false);
	EQUAL(game.level().map.cell(1, 0).visible, true);
	EQUAL(game.level().map.cell(2, 0).visible, true);
	EQUAL(game.level().map.cell(1, 1).visible, true);
	EQUAL(game.level().map.cell(2, 1).visible, true);
}

TEST_FIXTURE(LevelForSeeing, should_not_see_through_opaque_cells)
{
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(0, 1).visible, false);
}


TEST(should_erase_dead_monsters)
{
	DummyDungeon dungeon;
	Game game(&dungeon);
	game.level() = Level(2, 2);
	const MonsterType * monster_alive = game.monster_types.insert(MonsterType::Builder("alive").max_hp(100).sprite(1));
	const MonsterType * monster_dead = game.monster_types.insert(MonsterType::Builder("dead").max_hp(100).sprite(2));
	game.level().monsters.push_back(Monster::Builder(monster_alive).pos(Point(1, 1)).hp(0));
	game.level().monsters.push_back(Monster::Builder(monster_dead).pos(Point(1, 1)));
	game.level().erase_dead_monsters();
	EQUAL(game.level().monsters.size(), (unsigned)1);
	EQUAL(game.level().monsters[0].type->sprite, 2);
}

}

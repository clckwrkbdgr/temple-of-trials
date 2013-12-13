#include "../level.h"
#include "../objects.h"
#include "../game.h"
#include "../test.h"

SUITE(level) {

TEST(impassable_cells_should_be_impassable)
{
	Level level(1, 1);
	level.map.celltypes[0].passable = false;
	ASSERT(!level.is_passable(0, 0));
}

TEST(closed_doors_should_be_impassable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).openable().opened(false));
	ASSERT(!level.is_passable(1, 1));
}

TEST(opened_doors_should_be_passable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).openable().opened(true).passable());
	ASSERT(level.is_passable(1, 1));
}

TEST(containers_should_be_impassable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)));
	ASSERT(!level.is_passable(1, 1));
}

TEST(fountains_should_be_impassable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)));
	ASSERT(!level.is_passable(1, 1));
}

TEST(monsters_should_be_impassable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.monsters.push_back(Monster::Builder().pos(Point(1, 1)));
	ASSERT(!level.is_passable(1, 1));
}

TEST(items_should_be_passable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.items.push_back(Item::Builder().pos(Point(1, 1)));
	ASSERT(level.is_passable(1, 1));
}

TEST(traps_should_be_passable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).passable());
	ASSERT(level.is_passable(1, 1));
}

TEST(stairs_should_be_passable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).passable());
	ASSERT(level.is_passable(1, 1));
}

TEST(passable_cells_should_be_passable)
{
	Level level(2, 2);
	level.map.celltypes[0].passable = true;
	ASSERT(level.is_passable(1, 1));
}


TEST(opaque_cells_should_be_opaque)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = false;
	ASSERT(!level.is_transparent(1, 1));
}

TEST(closed_doors_should_be_opaque)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).openable().opened(false));
	ASSERT(!level.is_transparent(1, 1));
}

TEST(opened_doors_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).openable().opened(true).transparent());
	ASSERT(level.is_transparent(1, 1));
}

TEST(containers_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).transparent());
	ASSERT(level.is_transparent(1, 1));
}

TEST(fountains_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).transparent());
	ASSERT(level.is_transparent(1, 1));
}

TEST(monsters_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.monsters.push_back(Monster::Builder().pos(Point(1, 1)));
	ASSERT(level.is_transparent(1, 1));
}

TEST(items_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.items.push_back(Item::Builder().pos(Point(1, 1)));
	ASSERT(level.is_transparent(1, 1));
}

TEST(traps_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).transparent());
	ASSERT(level.is_transparent(1, 1));
}

TEST(stairs_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).transparent());
	ASSERT(level.is_transparent(1, 1));
}

TEST(transparent_cells_should_be_transparent)
{
	Level level(2, 2);
	level.map.celltypes[0].transparent = true;
	ASSERT(level.is_transparent(1, 1));
}


TEST(monster_should_be_on_top_of_all)
{
	Level level(2, 2);
	level.map.celltypes[0].sprite = 1;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).sprite(2));
	level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).sprite(3));
	level.items.push_back(Item::Builder().pos(Point(1, 1)).sprite(4));
	int sprite = level.get_sprite_at(Point(1, 1));
	EQUAL(sprite, 3);
}

TEST(items_should_be_on_top_of_objects)
{
	Level level(2, 2);
	level.map.celltypes[0].sprite = 1;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).sprite(2));
	level.items.push_back(Item::Builder().pos(Point(1, 1)).sprite(4));
	int sprite = level.get_sprite_at(Point(1, 1));
	EQUAL(sprite, 4);
}

TEST(objects_should_be_below_everything)
{
	Level level(2, 2);
	level.map.celltypes[0].sprite = 1;
	level.objects.push_back(Object::Builder().pos(Point(1, 1)).sprite(2));
	int sprite = level.get_sprite_at(Point(1, 1));
	EQUAL(sprite, 2);
}


TEST(should_get_player_from_monsters)
{
	Level level(2, 2);
	level.monsters.push_back(Monster::Builder().faction(Monster::PLAYER));
	Monster & monster = level.get_player();
	EQUAL(monster.faction, Monster::PLAYER);
}

TEST(should_get_player_from_const_monsters)
{
	Level level(2, 2);
	level.monsters.push_back(Monster::Builder().faction(Monster::PLAYER));
	const Monster & monster = static_cast<const Level &>(level).get_player();
	EQUAL(monster.faction, Monster::PLAYER);
}


struct LevelWithPath {
	Level level;
	LevelWithPath()
		: level(4, 4)
	{
		level.map.add_cell_type(CellType::Builder().passable(true));
		level.map.add_cell_type(CellType::Builder().passable(false));
		int a[] = {
			1, 2, 1, 1,
			2, 1, 1, 2,
			1, 2, 2, 1,
			1, 2, 1, 2,
		};
		level.map.fill(a);
	}
};

TEST_FIXTURE(LevelWithPath, should_find_path_between_points)
{
	std::list<Control> path = level.find_path(Point(0, 3), Point(2, 3));
	EQUAL(path.size(), (unsigned)5);
	std::list<Control>::const_iterator it = path.begin();
	EQUAL((*it++).direction, Point(0, -1));
	EQUAL((*it++).direction, Point(1, -1));
	EQUAL((*it++).direction, Point(1,  0));
	EQUAL((*it++).direction, Point(1,  1));
	EQUAL((*it++).direction, Point(-1, 1));
}

TEST_FIXTURE(LevelWithPath, should_find_path_between_close_points)
{
	std::list<Control> path = level.find_path(Point(0, 3), Point(0, 2));
	EQUAL(path.size(), (unsigned)1);
	std::list<Control>::const_iterator it = path.begin();
	EQUAL((*it++).direction, Point(0, -1));
}

TEST_FIXTURE(LevelWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	std::list<Control> path = level.find_path(Point(0, 3), Point(0, 3));
	ASSERT(path.empty());
}

struct LevelForSeeing {
	Level level;
	LevelForSeeing()
		: level(3, 2)
	{
		level.map.add_cell_type(CellType::Builder().sprite(1).passable(true).transparent(true));
		level.map.add_cell_type(CellType::Builder().sprite(2).passable(false).transparent(false));
		int a[] = {
			2, 1, 2,
			1, 2, 1,
		};
		level.map.fill(a);
		level.monsters.push_back(Monster::Builder().pos(Point(2, 1)).faction(Monster::PLAYER).sight(3).sprite(100));
	}
};

TEST_FIXTURE(LevelForSeeing, should_store_seen_sprites)
{
	level.invalidate_fov(level.get_player());
	EQUAL(level.map.cell_properties(1, 0).seen_sprite, 1);
	EQUAL(level.map.cell_properties(2, 0).seen_sprite, 2);
	EQUAL(level.map.cell_properties(1, 1).seen_sprite, 2);
	EQUAL(level.map.cell_properties(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_remember_sprite_instead_of_content)
{
	level.invalidate_fov(level.get_player());

	level.get_player().pos = Point(0, 1);
	level.invalidate_fov(level.get_player());
	EQUAL(level.map.cell_properties(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_calculate_visible_area_within_sight_radius)
{
	level.invalidate_fov(level.get_player());
	EQUAL(level.map.cell_properties(0, 0).visible, false);
	EQUAL(level.map.cell_properties(0, 1).visible, false);
	EQUAL(level.map.cell_properties(1, 0).visible, true);
	EQUAL(level.map.cell_properties(2, 0).visible, true);
	EQUAL(level.map.cell_properties(1, 1).visible, true);
	EQUAL(level.map.cell_properties(2, 1).visible, true);
}

TEST_FIXTURE(LevelForSeeing, should_not_see_through_opaque_cells)
{
	level.invalidate_fov(level.get_player());
	EQUAL(level.map.cell_properties(0, 1).visible, false);
}


TEST(should_erase_dead_monsters)
{
	Level level(2, 2);
	level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).sprite(1));
	level.monsters.push_back(Monster::Builder().pos(Point(1, 1)).hp(100).sprite(2));
	level.monsters[0].hp = 0;
	level.erase_dead_monsters();
	EQUAL(level.monsters.size(), (unsigned)1);
	EQUAL(level.monsters[0].sprite, 2);
}

}

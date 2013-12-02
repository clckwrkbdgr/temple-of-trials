#include "../map.h"
#include "../test.h"

TEST(map_should_have_default_cell_type)
{
	Map map(1, 1);
	EQUAL(map.celltypes.size(), 1);
}

TEST(point_inside_map_should_be_valid)
{
	Map map(3, 3);
	Point point(1, 1);
	ASSERT(map.valid(point));
}

TEST(point_outside_map_should_be_invalid)
{
	Map map(3, 3);
	Point point(-1, 1);
	ASSERT(!map.valid(point));
}

TEST(cell_type_should_be_appended_to_the_end)
{
	Map map(3, 3);
	int new_type = map.add_cell_type(CellType());
	EQUAL(new_type, 1);
}


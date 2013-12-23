#include "../map.h"
#include "../test.h"

SUITE(map) {

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

}

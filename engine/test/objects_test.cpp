#include "../objects.h"
#include "../game.h"
#include "../test.h"

SUITE(objects) {

TEST(should_get_opened_sprite_when_object_is_opened)
{
	Object object = Object::Builder().opened_sprite(1).closed_sprite(2).openable().opened(true);
	EQUAL(object.get_sprite(), 1);
}

TEST(should_get_closed_sprite_when_object_is_closed)
{
	Object object = Object::Builder().opened_sprite(1).closed_sprite(2).openable().opened(false);
	EQUAL(object.get_sprite(), 2);
}

TEST(opened_object_should_be_passable)
{
	Object object = Object::Builder().opened(true).passable();
	ASSERT(object.is_passable());
}

TEST(closed_object_should_be_impassable)
{
	Object object = Object::Builder().opened(false);
	ASSERT(!object.is_passable());
}

TEST(object_should_be_impassable_by_default)
{
	Object object;
	ASSERT(!object.is_passable());
}

TEST(passable_object_should_be_passable)
{
	Object object = Object::Builder().passable();
	ASSERT(object.is_passable());
}

TEST(opened_object_should_be_transparent)
{
	Object object = Object::Builder().openable().opened(true).transparent();
	ASSERT(object.is_transparent());
}

TEST(closed_object_should_be_opaque)
{
	Object object = Object::Builder().openable().opened(false).transparent();
	ASSERT(!object.is_transparent());
}

TEST(transparent_object_should_be_transparent)
{
	Object object = Object::Builder().transparent();
	ASSERT(object.is_transparent());
}

TEST(object_should_be_opaque_by_default)
{
	Object object;
	ASSERT(!object.is_transparent());
}

TEST(negative_up_destination_should_be_exit_from_dungeon)
{
	Object object = Object::Builder().up_destination(-1);
	ASSERT(object.is_exit_up());
}

TEST(negative_down_destination_should_be_exit_from_dungeon)
{
	Object object = Object::Builder().down_destination(-1);
	ASSERT(object.is_exit_down());
}

}

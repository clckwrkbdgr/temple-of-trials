#include "../objects.h"
#include "../game.h"
#include "../test.h"

SUITE(objects) {

TEST(should_get_opened_sprite_when_object_is_opened)
{
	ObjectType type = ObjectType::Builder("test").opened_sprite(1).closed_sprite(2).openable();
	Object object = Object::Builder(&type).opened(true);
	EQUAL(object.get_sprite(), 1);
}

TEST(should_get_closed_sprite_when_object_is_closed)
{
	ObjectType type = ObjectType::Builder("test").opened_sprite(1).closed_sprite(2).openable();
	Object object = Object::Builder(&type).opened(false);
	EQUAL(object.get_sprite(), 2);
}

TEST(opened_object_should_be_passable)
{
	ObjectType type = ObjectType::Builder("test").openable().passable();
	Object object = Object::Builder(&type).opened(true);
	ASSERT(object.is_passable());
}

TEST(closed_object_should_be_impassable)
{
	ObjectType type = ObjectType::Builder("test").openable().passable();
	Object object = Object::Builder(&type).opened(false);
	ASSERT(!object.is_passable());
}

TEST(object_should_be_impassable_by_default)
{
	Object object;
	ASSERT(!object.is_passable());
}

TEST(passable_object_should_be_passable)
{
	ObjectType type = ObjectType::Builder("test").passable();
	Object object = Object::Builder(&type);
	ASSERT(object.is_passable());
}

TEST(opened_object_should_be_transparent)
{
	ObjectType type = ObjectType::Builder("test").openable().transparent();
	Object object = Object::Builder(&type).opened(true);
	ASSERT(object.is_transparent());
}

TEST(closed_object_should_be_opaque)
{
	ObjectType type = ObjectType::Builder("test").openable().transparent();
	Object object = Object::Builder(&type).opened(false);
	ASSERT(!object.is_transparent());
}

TEST(transparent_object_should_be_transparent)
{
	ObjectType type = ObjectType::Builder("test").transparent();
	Object object = Object::Builder(&type);
	ASSERT(object.is_transparent());
}

TEST(object_should_be_opaque_by_default)
{
	Object object;
	ASSERT(!object.is_transparent());
}

TEST(negative_up_destination_should_be_exit_from_dungeon)
{
	ObjectType type = ObjectType::Builder("test").transporting();
	Object object = Object::Builder(&type).up_destination(-1);
	ASSERT(object.is_exit_up());
}

TEST(negative_down_destination_should_be_exit_from_dungeon)
{
	ObjectType type = ObjectType::Builder("test").transporting();
	Object object = Object::Builder(&type).down_destination(-1);
	ASSERT(object.is_exit_down());
}

}

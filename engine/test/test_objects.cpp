#include "../objects.h"
#include "../game.h"
#include "../test.h"

SUITE(monsters) {

TEST(monster_with_nonzero_hp_should_be_alive)
{
	Monster monster;
	monster.hp = 1;
	ASSERT(!monster.is_dead());
}

TEST(monster_with_zero_hp_should_die)
{
	Monster monster;
	monster.hp = 0;
	ASSERT(monster.is_dead());
}

TEST(monster_without_equipment_should_have_base_damage)
{
	Monster monster;
	monster.hit_strength = 3;
	EQUAL(monster.damage(), 3);
}

TEST(monster_with_equipment_should_have_weapon_damage_instead)
{
	Monster monster;
	monster.hit_strength = 3;
	Item weapon;
	weapon.damage = 1;
	weapon.sprite = 1;
	monster.inventory.push_back(weapon);
	monster.wielded = 0;
	EQUAL(monster.damage(), 1);
}

TEST(should_get_monster_wielded_item)
{
	Monster monster;
	Item weapon;
	weapon.sprite = 1;
	weapon.damage = 1;
	monster.inventory.push_back(weapon);
	monster.wielded = 0;
	EQUAL(monster.wielded_item(), weapon);
}

TEST(should_modify_monster_wielded_item)
{
	Monster monster;
	Item weapon;
	weapon.damage = 1;
	weapon.sprite = 1;
	monster.inventory.push_back(weapon);
	monster.wielded = 0;
	monster.wielded_item().damage = 2;
	EQUAL(monster.inventory.front().damage, 2);
}

TEST(should_get_const_monster_wielded_item)
{
	Item weapon;
	weapon.damage = 1;
	weapon.sprite = 1;
	const Monster monster = Monster::Builder().item(weapon).wield(0);
	EQUAL(monster.wielded_item(), weapon);
}

TEST(should_get_monster_worn_item)
{
	Monster monster;
	Item armor;
	armor.sprite = 1;
	armor.wearable = true;
	monster.inventory.push_back(armor);
	monster.worn = 0;
	EQUAL(monster.worn_item(), armor);
}

TEST(should_modify_monster_worn_item)
{
	Monster monster;
	Item armor;
	armor.defence = 1;
	armor.sprite = 1;
	monster.inventory.push_back(armor);
	monster.worn = 0;
	monster.worn_item().defence = 2;
	EQUAL(monster.inventory.front().defence, 2);
}

TEST(should_get_const_monster_worn_item)
{
	Item armor;
	armor.sprite = 1;
	armor.wearable = true;
	const Monster monster = Monster::Builder().item(armor).wear(0);
	EQUAL(monster.worn_item(), armor);
}

}

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

}

#include "../objects.h"
#include "../game.h"
#include "../test.h"

SUITE(objects) {

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

TEST(should_get_door_opened_sprite_when_opened)
{
	Object door = Object::Builder().opened_sprite(1).closed_sprite(2).openable().opened(true);
	EQUAL(door.get_sprite(), 1);
}

TEST(should_get_door_closed_sprite_when_closed)
{
	Object door = Object::Builder().opened_sprite(1).closed_sprite(2).openable().opened(false);
	EQUAL(door.get_sprite(), 2);
}

TEST(opened_door_should_be_passable)
{
	Object door = Object::Builder().opened(true).passable();
	ASSERT(door.is_passable());
}

TEST(closed_door_should_be_impassable)
{
	Object door = Object::Builder().opened(false);
	ASSERT(!door.is_passable());
}

TEST(container_should_be_impassable)
{
	Object container;
	ASSERT(!container.is_passable());
}

TEST(fountain_should_be_impassable)
{
	Object fountain;
	ASSERT(!fountain.is_passable());
}

TEST(stairs_should_be_passable)
{
	Object stairs = Object::Builder().passable();
	ASSERT(stairs.is_passable());
}

TEST(trap_should_be_passable)
{
	Object trap;
	trap.passable = true;
	ASSERT(trap.is_passable());
}

TEST(opened_door_should_be_transparent)
{
	Object door = Object::Builder().openable().opened(true).transparent();
	ASSERT(door.is_transparent());
}

TEST(closed_door_should_be_opaque)
{
	Object door = Object::Builder().openable().opened(false).transparent();
	ASSERT(!door.is_transparent());
}

TEST(container_should_be_transparent)
{
	Object container;
	container.transparent = true;
	ASSERT(container.is_transparent());
}

TEST(fountain_should_be_transparent)
{
	Object fountain;
	fountain.transparent = true;
	ASSERT(fountain.is_transparent());
}

TEST(stairs_should_be_transparent)
{
	Object stairs;
	stairs.transparent = true;
	ASSERT(stairs.is_transparent());
}

TEST(trap_should_be_transparent)
{
	Object trap;
	trap.transparent = true;
	ASSERT(trap.is_transparent());
}

}

#include "../objects.h"
#include "../game.h"
#include "../test.h"

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
	const Monster monster = Monster::Builder().item(weapon).wield(0);
	EQUAL(monster.wielded_item(), weapon);
}

TEST(should_get_monster_worn_item)
{
	Monster monster;
	Item armor;
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
	armor.wearable = true;
	const Monster monster = Monster::Builder().item(armor).wear(0);
	EQUAL(monster.worn_item(), armor);
}

TEST(should_get_door_opened_sprite_when_opened)
{
	Door door = Door::Builder().opened_sprite(1).closed_sprite(2).opened(true);
	EQUAL(door.sprite(), 1);
}

TEST(should_get_door_closed_sprite_when_closed)
{
	Door door = Door::Builder().opened_sprite(1).closed_sprite(2).opened(false);
	EQUAL(door.sprite(), 2);
}


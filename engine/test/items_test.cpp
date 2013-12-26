#include "../items.h"
#include "../test.h"

SUITE(inventory) {

TEST(should_get_item)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	inventory.set_item(1, Item(&type));
	EQUAL(inventory.get_item(1).type->sprite, 1);
}

TEST(should_get_empty_item_when_out_of_bounds)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	inventory.set_item(1, Item(&type));
	ASSERT(!inventory.get_item(2).valid());
}

TEST(should_set_item)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	bool ok = inventory.set_item(1, Item(&type));
	ASSERT(ok);
	EQUAL(inventory.get_item(1).type->sprite, 1);
}

TEST(should_not_set_item_if_slot_is_too_large)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	bool ok = inventory.set_item(26, Item(&type));
	ASSERT(!ok);
}

TEST(should_unwield_any_item)
{
	Inventory inventory;
	inventory.wielded = 1;
	inventory.unwield();
	EQUAL(inventory.wielded, Inventory::NOTHING);
}

TEST(should_insert_at_first_empty_slot)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	ItemType type2 = ItemType::Builder("item2").sprite(2);
	inventory.set_item(1, Item(&type));
	int slot = inventory.insert(Item(&type2));
	EQUAL(slot, 0);
	EQUAL(inventory.get_item(0).type->sprite, 2);
}

TEST(should_not_insert_when_there_is_no_place)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	ItemType type2 = ItemType::Builder("item2").sprite(2);
	for(int i = 0; i < 26; ++i) {
		inventory.set_item(i, Item(&type));
	}
	int slot = inventory.insert(Item(&type2));
	EQUAL(slot, Inventory::NOTHING);
}

TEST(should_take_item_from_inventory)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	inventory.set_item(0, Item(&type));
	Item item = inventory.take_item(0);
	ASSERT(!inventory.get_item(0).valid());
	EQUAL(item.type->sprite, 1);
}

TEST(should_take_first_item_from_inventory)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1);
	inventory.set_item(0, Item(&type));
	Item item = inventory.take_first_item();
	ASSERT(!inventory.get_item(0).valid());
	EQUAL(item.type->sprite, 1);
}

TEST(should_not_take_first_item_from_inventory_if_empty)
{
	Inventory inventory;
	inventory.set_item(0, Item());
	Item item = inventory.take_first_item();
	ASSERT(!item.valid());
}

TEST(should_get_quest_items_when_carrying_one)
{
	Inventory inventory;
	ItemType type = ItemType::Builder("item").sprite(1).quest();
	inventory.insert(Item(&type));
	const Item & item = inventory.quest_item();
	ASSERT(item.valid());
	ASSERT(item.type->quest);
}

}


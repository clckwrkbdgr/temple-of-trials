#include "../items.h"
#include "../test.h"

SUITE(inventory) {

TEST(should_get_item)
{
	Inventory inventory;
	inventory.set_item(1, Item::Builder().sprite(1));
	EQUAL(inventory.get_item(1).sprite, 1);
}

TEST(should_get_empty_item_when_out_of_bounds)
{
	Inventory inventory;
	inventory.set_item(1, Item::Builder().sprite(1));
	ASSERT(!inventory.get_item(2).valid());
}

TEST(should_set_item)
{
	Inventory inventory;
	bool ok = inventory.set_item(1, Item::Builder().sprite(1));
	ASSERT(ok);
	EQUAL(inventory.get_item(1).sprite, 1);
}

TEST(should_not_set_item_if_slot_is_too_large)
{
	Inventory inventory;
	bool ok = inventory.set_item(26, Item::Builder().sprite(1));
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
	inventory.set_item(1, Item::Builder().sprite(1));
	int slot = inventory.insert(Item::Builder().sprite(2));
	EQUAL(slot, 0);
	EQUAL(inventory.get_item(0).sprite, 2);
}

TEST(should_not_insert_when_there_is_no_place)
{
	Inventory inventory;
	for(int i = 0; i < 26; ++i) {
		inventory.set_item(i, Item::Builder().sprite(i + 1));
	}
	int slot = inventory.insert(Item::Builder().sprite(2));
	EQUAL(slot, Inventory::NOTHING);
}

TEST(should_take_item_from_inventory)
{
	Inventory inventory;
	inventory.set_item(0, Item::Builder().sprite(1));
	Item item = inventory.take_item(0);
	ASSERT(!inventory.get_item(0).valid());
	EQUAL(item.sprite, 1);
}

TEST(should_take_first_item_from_inventory)
{
	Inventory inventory;
	inventory.set_item(0, Item::Builder().sprite(1));
	Item item = inventory.take_first_item();
	ASSERT(!inventory.get_item(0).valid());
	EQUAL(item.sprite, 1);
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
	inventory.insert(Item::Builder().quest().sprite(1));
	const Item & item = inventory.quest_item();
	ASSERT(item.valid());
	ASSERT(item.quest);
}

}


#include "../message.h"
#include "../test.h"

TEST(game_should_start_with_empty_messages)
{
	Messages messages;
	ASSERT(messages.messages.empty());
}

TEST(should_accept_only_non_empty_messages)
{
	Messages messages;
	messages.message("");
	ASSERT(messages.messages.empty());
}

TEST(should_add_messages)
{
	Messages messages;
	messages.message("hello");
	EQUAL(messages.messages.size(), (unsigned)1);
}

TEST(should_titlecase_messages)
{
	Messages messages;
	messages.message("hello");
	EQUAL(messages.messages.front(), "Hello");
}


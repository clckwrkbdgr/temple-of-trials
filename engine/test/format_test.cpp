#include "../util.h"
#include "../format.h"
#include "../test.h"

SUITE(format) {

TEST(should_convert_int_to_string)
{
	std::string result = to_string(int(1));
	EQUAL(result, "1");
	result = to_string(int(-1));
	EQUAL(result, "-1");
}

TEST(should_convert_unsigned_to_string)
{
	std::string result = to_string(unsigned(1));
	EQUAL(result, "1");
}

TEST(should_convert_unsigned_long_to_string)
{
	std::string result = to_string((unsigned long)1);
	EQUAL(result, "1");
}

TEST(should_convert_char_to_string)
{
	std::string result = to_string(char('a'));
	EQUAL(result, "a");
}

TEST(should_convert_string_to_string)
{
	std::string result = to_string(std::string("hello"));
	EQUAL(result, "hello");
}

TEST(should_convert_vector_to_string)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3)(4);
	std::string result = to_string(v);
	EQUAL(result, "1|2|3|4");
}

TEST(should_convert_list_to_string)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3)(4);
	std::list<int> l(v.begin(), v.end());
	std::string result = to_string(l);
	EQUAL(result, "1|2|3|4");
}

}

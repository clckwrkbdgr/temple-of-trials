#include "../util.h"
#include "../map.h"
#include "../test.h"

SUITE(util) {

TEST(should_return_sign_of_positive_number)
{
	int result = sign(10);
	EQUAL(result, 1);
}

TEST(should_return_sign_of_negative_number)
{
	int result = sign(-10);
	EQUAL(result, -1);
}

TEST(should_return_sign_of_zero)
{
	int result = sign(0);
	EQUAL(result, 0);
}

TEST(should_iterate_for_each_value_in_vector)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3);
	int j = 1;
	foreach(int i, v) {
		EQUAL(i, j);
		++j;
	}
}

TEST(should_iterate_for_each_value_in_vector_by_ref)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3);
	foreach(int & i, v) {
		i *= i;
	}
	int a[] = {1, 4, 9};
	EQUAL(v, make_vector(a));
}

TEST(should_iterate_for_each_char_in_string)
{
	std::string s = "run";
	int j = 0;
	foreach(char ch, s) {
		switch(j) {
			case 0: EQUAL(ch, 'r'); break;
			case 1: EQUAL(ch, 'u'); break;
			case 2: EQUAL(ch, 'n'); break;
		}
		++j;
	}
}

TEST(should_iterate_for_each_char_in_string_by_ref)
{
	std::string s = "abc";
	foreach(char & ch, s) {
		++ch;
	}
	EQUAL(s, "bcd");
}

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

TEST(should_push_back_value_to_vector)
{
	std::vector<int> v;
	v << 99;
	EQUAL(v.size(), (unsigned)1);
	EQUAL(v[0], 99);
}

TEST(should_push_back_c_string_to_vector)
{
	std::vector<std::string> stringlist;
	stringlist << "hello";
	EQUAL(stringlist.size(), (unsigned)1);
	EQUAL(stringlist[0], "hello");
}

TEST(should_make_vector_from_array)
{
	int a[] = {1, 2, 3};
	std::vector<int> v = make_vector(a);
	EQUAL(v.size(), (unsigned)3);
	EQUAL(v[0], 1);
	EQUAL(v[1], 2);
	EQUAL(v[2], 3);
}

TEST(should_make_vector_using_init_chain)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3);
	EQUAL(v.size(), (unsigned)3);
	EQUAL(v[0], 1);
	EQUAL(v[1], 2);
	EQUAL(v[2], 3);
}

TEST(should_compare_containers)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3);
	std::list<int> l(v.begin(), v.end());
	ASSERT(equal_containers(v.begin(), v.end(), l.begin(), l.end()));
}

TEST(should_compare_unequal_containers_unequal)
{
	std::vector<int> v = MakeVector<int>(1)(2)(3);
	std::list<int> l(v.begin(), v.end());
	v.push_back(4);
	ASSERT(!equal_containers(v.begin(), v.end(), l.begin(), l.end()));
}


struct Value {
	struct Type {
		std::string id;
		int sprite;
		Type() : sprite(0) {}
		Type(const std::string & type_id, int type_sprite) : id(type_id), sprite(type_sprite) {}
	};
	const Type * type;
	Value(const Type * value_type = 0) : type(value_type) {}
};

TEST(should_assume_non_null_type_pointer_as_valid)
{
	Value::Type type("type", 1);
	TypePtr<Value::Type> ptr(&type);
	ASSERT(ptr.valid());
}

TEST(should_assume_null_type_pointer_as_invalid)
{
	TypePtr<Value::Type> ptr;
	ASSERT(!ptr.valid());
}

TEST(should_return_type_from_pointer_if_valid)
{
	Value::Type type("type", 1);
	TypePtr<Value::Type> ptr(&type);
	EQUAL(ptr->id, "type");
}

TEST(should_return_default_type_from_pointer_if_invalid)
{
	TypePtr<Value::Type> ptr;
	ASSERT(ptr->id.empty());
}

TEST(should_get_type_for_value)
{
	TypeRegistry<Value> reg;
	reg.insert(Value::Type("test", 1));
	ASSERT(reg.get("test"));
}

TEST(should_return_default_empty_type_for_unknown_cell_type)
{
	TypeRegistry<Value> reg;
	reg.insert(Value::Type("known", 1));
	ASSERT(!reg.get("unknown"));
}

}

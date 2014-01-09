#include "../util.h"
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
	std::vector<int> v; v << 1 << 2 << 3;
	int j = 1;
	foreach(int i, v) {
		EQUAL(i, j);
		++j;
	}
}

TEST(should_iterate_for_each_value_in_vector_by_ref)
{
	std::vector<int> v; v << 1 << 2 << 3;
	foreach(int & i, v) {
		i *= i;
	}
	TEST_CONTAINER(v, i) {
		EQUAL(i, 1);
	} NEXT(i) {
		EQUAL(i, 4);
	} NEXT(i) {
		EQUAL(i, 9);
	} DONE(i);
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


struct Value {
	struct Type {
		std::string id;
		int sprite;
		Type() : sprite(0) {}
		Type(const std::string & type_id, int type_sprite) : id(type_id), sprite(type_sprite) {}
		struct Builder;
	};
	const Type * type;
	Value(const Type * value_type = 0) : type(value_type) {}
};
struct Value::Type::Builder {
	Value::Type value_result;
	Value::Type & result;
	Builder(const std::string & type_id) : value_result(type_id, 0), result(value_result) {}
	Builder(Value::Type & type) : result(type) {}
	operator Value::Type() { return result; }
	Builder & sprite(const int & value) { result.sprite = value; return *this; }
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

TEST(should_return_builder_when_inserting)
{
	TypeRegistry<Value> reg;
	reg.insert("test").sprite(1);
	ASSERT(reg.get("test"));
	EQUAL(reg.get("test")->sprite, 1);
}

}

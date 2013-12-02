#include "../util.h"
#include <list>
#include <iostream>
#include <cassert>

typedef void(*TestFunction)();
struct AddTest {
	std::string name;
	TestFunction impl;
	AddTest(const std::string & test_name, TestFunction test_function);
};
std::list<AddTest> & all_tests()
{
	static std::list<AddTest> tests;
	return tests;
}
AddTest::AddTest(const std::string & test_name, TestFunction test_function)
	: name(test_name), impl(test_function)
{
	all_tests().push_back(*this);
}
struct TestException {
	std::string filename;
	int line;
	std::string what;
	TestException(const std::string & ex_filename, int ex_linenumber, const std::string & message)
		: filename(ex_filename), line(ex_linenumber), what(message) {}
};
#define TEST(test_name) \
	void test_name(); \
	AddTest add_test_##test_name(#test_name, test_name); \
	void test_name()
#define EQUAL(a, b) \
	do { if(a != b) { throw TestException(__FILE__, __LINE__, #a " (" + to_string(a) + ") != " #b " (" + to_string(b) + ")"); } } while(0);
#define ASSERT(expression) \
	do { if(!(expression)) { throw TestException(__FILE__, __LINE__, "failed assertion: " #expression ); } } while(0);

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
	std::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	int j = 1;
	foreach(int i, v) {
		EQUAL(i, j);
		++j;
	}
}

TEST(should_iterate_for_each_value_in_vector_by_ref)
{
	std::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	foreach(int & i, v) {
		i *= i;
	}
	EQUAL(v[0], 1);
	EQUAL(v[1], 4);
	EQUAL(v[2], 9);
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

TEST(should_convert_point_to_string)
{
	std::string result = to_string(Point(10, 0));
	EQUAL(result, "(10, 0)");
}

TEST(should_make_point_from_two_coords)
{
	Point p(1, 2);
	EQUAL(p.x, 1);
	EQUAL(p.y, 2);
}

TEST(should_consider_non_zero_point_valid)
{
	Point p(1, 2);
	ASSERT(p);
}

TEST(should_consider_zero_point_invalid)
{
	Point p(0, 0);
	ASSERT(!p);
}

TEST(should_make_point_from_point)
{
	Point p(1, 2);
	Point d = p;
	EQUAL(p.x, d.x);
	EQUAL(p.y, d.y);
}

TEST(should_add_point_to_point)
{
	Point a(1, 2), b(2, 5);
	Point c = a + b;
	EQUAL(c.x, 3);
	EQUAL(c.y, 7);
}

TEST(should_subtract_point_from_point)
{
	Point a(1, 2), b(2, 5);
	Point c = a - b;
	EQUAL(c.x, -1);
	EQUAL(c.y, -3);
}

TEST(should_divide_point_by_factor)
{
	Point a(16, 25);
	Point c = a / 4;
	EQUAL(c.x, 4);
	EQUAL(c.y, 6);
}

TEST(should_multiply_point_by_factor)
{
	Point a(4, 6);
	Point c = a * 4;
	EQUAL(c.x, 16);
	EQUAL(c.y, 24);
}

struct PosInt {
	int value;
	Point pos;
	PosInt() : value(0) {}
	PosInt(int _value, const Point & _pos) : value(_value), pos(_pos) {}
};

TEST(should_find_item_in_vector)
{
	PosInt a[] = {PosInt(1, Point(1, 0)), PosInt(2, Point(0, 1)), PosInt(3, Point(0, 0))};
	std::vector<PosInt> v(a, a + 3);
	PosInt & i = find_at(v, Point(0, 1));
	EQUAL(i.value, 2);
}

TEST(should_change_found_item_in_vector)
{
	PosInt a[] = {PosInt(1, Point(1, 0)), PosInt(2, Point(0, 1)), PosInt(3, Point(0, 0))};
	std::vector<PosInt> v(a, a + 3);
	PosInt & i = find_at(v, Point(0, 1));
	i.value = 4;
	EQUAL(v[1].value, 4);
}

TEST(should_return_empty_item_if_not_found_in_vector)
{
	PosInt a[] = {PosInt(1, Point(1, 0)), PosInt(2, Point(0, 1)), PosInt(3, Point(0, 0))};
	std::vector<PosInt> v(a, a + 3);
	PosInt & i = find_at(v, Point(1, 1));
	EQUAL(i.value, 0);
	ASSERT(!i.pos);
}

TEST(should_find_item_in_const_vector)
{
	PosInt a[] = {PosInt(1, Point(1, 0)), PosInt(2, Point(0, 1)), PosInt(3, Point(0, 0))};
	const std::vector<PosInt> v(a, a + 3);
	PosInt i = find_at(v, Point(0, 1));
	EQUAL(i.value, 2);
}

TEST(should_return_empty_item_if_not_found_in_const_vector)
{
	PosInt a[] = {PosInt(1, Point(1, 0)), PosInt(2, Point(0, 1)), PosInt(3, Point(0, 0))};
	const std::vector<PosInt> v(a, a + 3);
	PosInt i = find_at(v, Point(1, 1));
	EQUAL(i.value, 0);
	ASSERT(!i.pos);
}

TEST(should_push_back_value_to_vector)
{
	std::vector<int> v;
	v << 99;
	EQUAL(v.size(), 1);
	EQUAL(v[0], 99);
}

TEST(should_push_back_c_string_to_vector)
{
	std::vector<std::string> stringlist;
	stringlist << "hello";
	EQUAL(stringlist.size(), 1);
	EQUAL(stringlist[0], "hello");
}

TEST(should_compute_distance_between_points)
{
	Point a(1, 2), b(11, 12);
	int d = distance(a, b);
	EQUAL(d, 14);
}

TEST(should_compute_distance_between_points_on_x_axis)
{
	Point a(1, 3), b(1, 13);
	int d = distance(a, b);
	EQUAL(d, 10);
}

TEST(should_compute_distance_between_points_on_y_axis)
{
	Point a(1, 3), b(13, 3);
	int d = distance(a, b);
	EQUAL(d, 12);
}

TEST(should_return_zero_for_same_points)
{
	Point a(1, 3), b(1, 3);
	int d = distance(a, b);
	EQUAL(d, 0);
}

TEST(should_return_one_for_close_point)
{
	Point a(1, 3), b(2, 4);
	int d = distance(a, b);
	EQUAL(d, 1);
}

void test_util()
{
	for(std::list<AddTest>::const_iterator test = all_tests().begin(); test != all_tests().end(); ++test) {
		std::cout << "Check: " << test->name << std::endl;
		try {
			test->impl();
		} catch(const TestException & e) {
			std::cerr << e.filename << ":" << e.line << ": " << e.what << std::endl;
		}
	}
}


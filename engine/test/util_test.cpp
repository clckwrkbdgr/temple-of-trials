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

TEST(should_convert_point_to_string)
{
	std::string result = to_string(Point(10, 0));
	EQUAL(result, "(10, 0)");
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
	std::vector<PosInt> v = MakeVector<PosInt>(PosInt(1, Point(1, 0)))(PosInt(2, Point(0, 1)))(PosInt(3, Point(0, 0)));
	PosInt & i = find_at(v, Point(0, 1));
	EQUAL(i.value, 2);
}

TEST(should_change_found_item_in_vector)
{
	std::vector<PosInt> v = MakeVector<PosInt>(PosInt(1, Point(1, 0)))(PosInt(2, Point(0, 1)))(PosInt(3, Point(0, 0)));
	PosInt & i = find_at(v, Point(0, 1));
	i.value = 4;
	EQUAL(v[1].value, 4);
}

TEST(should_return_empty_item_if_not_found_in_vector)
{
	std::vector<PosInt> v = MakeVector<PosInt>(PosInt(1, Point(1, 0)))(PosInt(2, Point(0, 1)))(PosInt(3, Point(0, 0)));
	PosInt & i = find_at(v, Point(1, 1));
	EQUAL(i.value, 0);
	ASSERT(!i.pos);
}

TEST(should_find_item_in_const_vector)
{
	const std::vector<PosInt> v = MakeVector<PosInt>(PosInt(1, Point(1, 0)))(PosInt(2, Point(0, 1)))(PosInt(3, Point(0, 0)));
	PosInt i = find_at(v, Point(0, 1));
	EQUAL(i.value, 2);
}

TEST(should_return_empty_item_if_not_found_in_const_vector)
{
	const std::vector<PosInt> v = MakeVector<PosInt>(PosInt(1, Point(1, 0)))(PosInt(2, Point(0, 1)))(PosInt(3, Point(0, 0)));
	PosInt i = find_at(v, Point(1, 1));
	EQUAL(i.value, 0);
	ASSERT(!i.pos);
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

}

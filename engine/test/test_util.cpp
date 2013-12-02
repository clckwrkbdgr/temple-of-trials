#include "../util.h"
#include <map>
#include <iostream>
#include <cassert>

typedef void(*TestFunction)();
static std::map<std::string, TestFunction> tests;
struct AddTest {
	AddTest(const std::string & test_name, TestFunction test_function)
	{
		tests[test_name] = test_function;
	}
};
#define TEST(test_name) \
	void test_name(); \
	AddTest add_test_##test_name(#test_name, test_name); \
	void test_name()

TEST(should_return_sign_of_positive_number)
{
	int result = sign(10);
	assert(result == 1);
}

TEST(should_return_sign_of_negative_number)
{
	int result = sign(-10);
	assert(result == -1);
}

TEST(should_return_sign_of_zero)
{
	int result = sign(0);
	assert(result == 0);
}

/*
should_iterate_for_each_value_in_vector
should_iterate_for_each_char_in_string
should_convert_int_to_string
should_convert_unsigned_to_string
should_convert_unsigned_long_to_string
should_convert_char_to_string
should_convert_string_to_string
should_convert_point_to_string
should_make_point_from_two_coords
should_make_point_from_point
should_add_point_to_point
should_subtract_point_to_point
should_divide_point_by_factor
should_multiply_point_by_factor
should_find_item_in_vector
should_return_empty_item_if_not_found_in_vector
should_find_item_in_const_vector
should_return_empty_item_if_not_found_in_const_vector
should_push_back_value_to_vector
should_push_back_c_string_to_vector
should_compute_distance_between_points_on_x_axis
should_compute_distance_between_points_on_y_axis
should_return_zero_for_same_points
should_return_one_for_close_point
*/

void test_util()
{
	for(std::map<std::string, TestFunction>::const_iterator test = tests.begin(); test != tests.end(); ++test) {
		std::cout << test->first << std::endl;
		test->second();
	}
}


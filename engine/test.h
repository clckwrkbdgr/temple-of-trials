#pragma once
#include <list>
#include <iostream>

typedef void(*TestFunction)();

struct AddTest {
	const char * suite;
	const char * name;
	TestFunction impl;
	AddTest(const char * test_suite, const char * test_name, TestFunction test_function);
};

std::list<AddTest> & all_tests();

struct TestException {
	const char * filename;
	const char * line;
	std::string what;
	TestException(const char * ex_filename, const char * ex_linenumber, const std::string & message);
};

const char * current_suite_name();
#define SUITE(suite_name) \
	namespace Suite_##suite_name { \
		const char * current_suite_name() { return #suite_name; } \
	} \
	namespace Suite_##suite_name

#define TEST(test_name) \
	void test_name(); \
	AddTest add_test_##test_name(current_suite_name(), #test_name, test_name); \
	void test_name()

template<class A, class B>
void test_equal(const A & a, const B & b, const char * a_string, const char * b_string, const char * file, const char * line)
{
	if(a != b) {
		throw TestException(file, line, std::string(a_string) + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define STR(x) #x
#define EQUAL(a, b) \
	test_equal(a, b, #a, #b, __FILE__, STR(__LINE__))

#define FAIL(message) \
	throw TestException(__FILE__, STR(__LINE__), message)

#define ASSERT(expression) \
	do { if(!(expression)) { throw TestException(__FILE__, STR(__LINE__), "failed assertion: " #expression ); } } while(0)

void run_all_tests(int argc, char ** argv);


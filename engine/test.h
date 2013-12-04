#pragma once
#include <list>
#include <iostream>

typedef void(*TestFunction)();

struct AddTest {
	std::string suite;
	std::string name;
	TestFunction impl;
	AddTest(const std::string & test_suite, const std::string & test_name, TestFunction test_function);
	std::string get_full_name() const;
};

std::list<AddTest> & all_tests();

struct TestException {
	std::string filename;
	int line;
	std::string what;
	TestException(const std::string & ex_filename, int ex_linenumber, const std::string & message);
};

std::string current_suite_name();
#define SUITE(suite_name) \
	namespace Suite_##suite_name { \
		std::string current_suite_name() { return #suite_name; } \
	} \
	namespace Suite_##suite_name

#define TEST(test_name) \
	void test_name(); \
	AddTest add_test_##test_name(current_suite_name(), #test_name, test_name); \
	void test_name()

template<class A, class B>
void test_equal(const A & a, const B & b, const std::string & a_string, const std::string & b_string, const std::string & file, int line)
{
	if(a != b) {
		throw TestException(file, line, a_string + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define EQUAL(a, b) \
	test_equal(a, b, #a, #b, __FILE__, __LINE__)

#define FAIL(message) \
	do { throw TestException(__FILE__, __LINE__, message); } while(0)

#define ASSERT(expression) \
	do { if(!(expression)) { throw TestException(__FILE__, __LINE__, "failed assertion: " #expression ); } } while(0)

void run_all_tests(int argc, char ** argv);


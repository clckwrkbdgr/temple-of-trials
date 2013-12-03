#pragma once
#include <list>
#include <iostream>

typedef void(*TestFunction)();

struct AddTest {
	std::string name;
	TestFunction impl;
	AddTest(const std::string & test_name, TestFunction test_function);
};

std::list<AddTest> & all_tests();

struct TestException {
	std::string filename;
	int line;
	std::string what;
	TestException(const std::string & ex_filename, int ex_linenumber, const std::string & message);
};
#define TEST(test_name) \
	void test_name(); \
	AddTest add_test_##test_name(#test_name, test_name); \
	void test_name()
#define EQUAL(a, b) \
	do { if(a != b) { throw TestException(__FILE__, __LINE__, #a " (" + to_string(a) + ") != " #b " (" + to_string(b) + ")"); } } while(0);
#define ASSERT(expression) \
	do { if(!(expression)) { throw TestException(__FILE__, __LINE__, "failed assertion: " #expression ); } } while(0);

void run_all_tests(int argc, char ** argv);


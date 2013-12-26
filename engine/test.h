#pragma once
#include "format.h"
#include <list>
#include <iostream>

struct Test {
	const char * suite;
	const char * name;
	const char * filename;
	int line;
	Test(const char * test_suite, const char * test_name, const char * filename, int line);
	virtual ~Test() {}
	virtual void run() = 0;
	bool specified(int argc, char ** argv) const;
};

std::list<Test*> & all_tests();

struct TestException {
	const char * filename;
	int line;
	std::string what;
	TestException(const char * ex_filename, int ex_linenumber, const std::string & message);
};

const char * current_suite_name();
#define SUITE(suite_name) \
	namespace Suite_##suite_name { \
		const char * current_suite_name() { return #suite_name; } \
	} \
	namespace Suite_##suite_name

#define TEST(test_name) \
	class Test_##test_name : public Test { \
	public: \
		Test_##test_name(const char * suite, const char * name) : Test(suite, name, __FILE__, __LINE__) {} \
		virtual void run(); \
	}; \
	Test_##test_name test_##test_name(current_suite_name(), #test_name); \
	void Test_##test_name::run()

#define TEST_FIXTURE(fixture_name, test_name) \
	class Fixture_##fixture_name##test_name : public fixture_name { \
	public: \
		void run(); \
	}; \
	TEST(test_name) \
	{ \
		Fixture_##fixture_name##test_name fixture; \
		fixture.run(); \
	} \
	void Fixture_##fixture_name##test_name::run()

template<class A, class B>
void test_equal(const A & a, const B & b, const char * a_string, const char * b_string, const char * file, int line)
{
	if(a != b) {
		throw TestException(file, line, std::string(a_string) + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define EQUAL(a, b) \
	test_equal(a, b, #a, #b, __FILE__, __LINE__)

template<class ContainerA, class ContainerB>
void test_equal_containers(const ContainerA & a, const ContainerB & b, const char * a_string, const char * b_string, const char * file, int line)
{
	if(!equal_containers(a.begin(), a.end(), b.begin(), b.end())) {
		throw TestException(file, line, std::string(a_string) + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define EQUAL_CONTAINERS(a, b) \
	test_equal_containers(a, b, #a, #b, __FILE__, __LINE__)

#define FAIL(message) \
	throw TestException(__FILE__, __LINE__, message)

#define ASSERT(expression) \
	do { if(!(expression)) { throw TestException(__FILE__, __LINE__, "failed assertion: " #expression ); } } while(0)

int run_all_tests(int argc, char ** argv);


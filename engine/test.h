#pragma once
#include <list>
#include <iostream>

struct Test {
	const char * suite;
	const char * name;
	Test(const char * test_suite, const char * test_name);
	virtual ~Test() {}
	virtual void run() = 0;
	bool specified(int argc, char ** argv) const;
};

std::list<Test*> & all_tests();

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
	class Test_##test_name : public Test { \
	public: \
		Test_##test_name(const char * suite, const char * name) : Test(suite, name) {} \
		virtual void run(); \
	}; \
	Test_##test_name test_##test_name(current_suite_name(), #test_name); \
	void Test_##test_name::run()

#define TEST_FIXTURE(fixture_name, test_name) \
	class Fixture_##fixture_name##test_name : public fixture_name { \
	public: \
		void run(); \
	}; \
	class Test_##test_name : public Test {  \
	public:  \
		Test_##test_name(const char * suite, const char * name) : Test(suite, name) {}  \
		virtual void run();  \
	};  \
	Test_##test_name test_##test_name(current_suite_name(), #test_name);  \
	void Test_##test_name::run() \
	{ \
		Fixture_##fixture_name##test_name fixture; \
		fixture.run(); \
	} \
	void Fixture_##fixture_name##test_name::run()

template<class A, class B>
void test_equal(const A & a, const B & b, const char * a_string, const char * b_string, const char * file, const char * line)
{
	if(a != b) {
		throw TestException(file, line, std::string(a_string) + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define STR_VALUE(x) #x
#define STR(x) STR_VALUE(x)
#define EQUAL(a, b) \
	test_equal(a, b, #a, #b, __FILE__, STR(__LINE__))

#define FAIL(message) \
	throw TestException(__FILE__, STR(__LINE__), message)

#define ASSERT(expression) \
	do { if(!(expression)) { throw TestException(__FILE__, STR(__LINE__), "failed assertion: " #expression ); } } while(0)

int run_all_tests(int argc, char ** argv);


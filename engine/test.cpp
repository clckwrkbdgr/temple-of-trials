#include "test.h"
#include "util.h"

std::list<Test*> & all_tests()
{
	static std::list<Test*> tests;
	return tests;
}

Test::Test(const char * test_suite, const char * test_name)
	: suite(test_suite), name(test_name)
{
	all_tests().push_back(this);
}

bool Test::specified(int argc, char ** argv) const
{
	for(int i = 1; i < argc; ++i) {
		if(strcmp(name, argv[i]) == 0 || strcmp(suite, argv[i]) == 0) {
			return true;
		}
	}
	return false;
}

TestException::TestException(const char * ex_filename, const char * ex_linenumber, const std::string & message)
	: filename(ex_filename), line(ex_linenumber), what(message)
{
}

const char * current_suite_name()
{
	return "";
}

int run_all_tests(int argc, char ** argv)
{
	bool tests_specified = argc > 1;
	int total_test_count = 0;
	int passed_tests = 0;
	for(std::list<Test*>::iterator it = all_tests().begin(); it != all_tests().end(); ++it) {
		Test * test = *it;
		if(tests_specified && !test->specified(argc, argv)) {
			continue;
		}
		++total_test_count;
		bool ok = true;
		std::string exception_text;
		std::string test_name = test->suite;
		test_name += std::string(test_name.empty() ? "" : " :: ") + test->name;
		try {
			test->run();
		} catch(const TestException & e) {
			ok = false;
			exception_text = std::string(e.filename) + ":" + e.line + ": " + e.what;
		} catch(const std::exception & e) {
			ok = false;
			exception_text = e.what();
		} catch(...) {
			ok = false;
			exception_text = "Unknown exception caught.";
		}
		if(ok) {
			std::cout << "[ OK ] " << test_name << std::endl;
			++passed_tests;
		} else {
			std::cout << "[FAIL] " << test_name << std::endl;
			std::cerr << exception_text << std::endl;
		}
	}
	int failed_tests = total_test_count - passed_tests;
	if(total_test_count == 0) {
		std::cout << "No tests to run." << std::endl;
	} else if(failed_tests == 0) {
		std::cout << "All tests are passed." << std::endl;
	} else {
		std::cout << failed_tests << ((failed_tests % 10 == 1) ? " test" : " tests") << " failed!" << std::endl;
	}
	return failed_tests;
}


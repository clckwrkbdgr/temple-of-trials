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

TestException::TestException(const char * ex_filename, const char * ex_linenumber, const std::string & message)
	: filename(ex_filename), line(ex_linenumber), what(message)
{
}

const char * current_suite_name()
{
	return "";
}

void run_all_tests(int argc, char ** argv)
{
	bool tests_specified = argc > 1;
	bool tests_found = false;
	bool all_tests_are_ok = true;
	for(std::list<Test*>::iterator it = all_tests().begin(); it != all_tests().end(); ++it) {
		Test * test = *it;
		if(tests_specified) {
			bool found = false;
			for(int i = 1; i < argc; ++i) {
				if(strcmp(test->name, argv[i]) == 0 || strcmp(test->suite, argv[i]) == 0) {
					found = true;
					break;
				}
			}
			if(!found) {
				continue;
			}
		}
		tests_found = true;
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
		std::cout << (ok ? "[ OK ] " : "[FAIL] ") << test_name << std::endl;
		if(!ok) {
			std::cout << exception_text << std::endl;
			all_tests_are_ok = false;
		}
	}
	if(!tests_found) {
		std::cout << "No tests to run." << std::endl;
	} else if(all_tests_are_ok) {
		std::cout << "All tests are passed." << std::endl;
	} else {
		std::cout << "Some tests are failed!" << std::endl;
	}
}


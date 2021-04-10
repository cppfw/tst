#pragma once

#include <string>
#include <map>
#include <mutex>

#include "tester.hpp"
#include "suite.hpp"

namespace tst{
class junit{
private:
	std::mutex mutex;
	decltype(tester::suites)& suites;
public:
	junit(decltype(tester::suites)& suites) :
			suites(suites)
	{}

	// thread safe
	void set_result(
			const std::string& suite_name,
			const std::string& test_name,
			suite::status result,
			std::string&& error_message = std::string()
		);

};

}

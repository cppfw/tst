#pragma once

#include <string>
#include <map>
#include <mutex>

#include "tester.hpp"
#include "suite.hpp"

namespace tst{
class reporter{
private:
	std::mutex mutex;
	const decltype(tester::suites)& suites;
public:
	reporter(decltype(suites)& suites) :
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

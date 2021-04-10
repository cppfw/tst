#pragma once

#include <string>
#include <map>
#include <mutex>

#include "suite.hpp"

namespace tst{
class junit{
private:
	std::mutex mutex;

public:

	// thread safe
	void set_result(
			const std::string& suite_name,
			const std::string& test_name,
			suite::status result,
			std::string&& error_message = std::string()
		);

};

}

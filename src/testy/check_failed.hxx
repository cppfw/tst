#pragma once

#include <string>
#include <sstream>

namespace testy{

class check_failed{
public:
	const std::string file;
	const size_t line;
	const std::string message;

	check_failed(const std::string& message, const std::pair<const char*, size_t>& source_location) :
			file(source_location.first),
			line(source_location.second),
			message(message)
	{}
};

}

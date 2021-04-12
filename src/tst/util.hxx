#pragma once

#include <string>

#include "application.hpp"

namespace tst{

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

struct full_id{
	const std::string& suite;
	const std::string& test;
};

void validate_id(const std::string& id);

decltype(create_application)* load_create_application_function();

}

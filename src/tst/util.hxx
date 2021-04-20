#pragma once

#include <string>

#include <utki/debug.hpp>

namespace tst{

class check_failed{
public:
	const utki::source_location source_location;
	const std::string message;

	check_failed(std::string&& message, utki::source_location&& source_location) :
			source_location(std::move(source_location)),
			message(std::move(message))
	{}
};

struct full_id{
	const std::string& suite;
	const std::string& test;
};

inline bool is_valid_id_char(char c){
	return ('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			('0' <= c && c <= '9') ||
			c == '_' ||
			c == '[' ||
			c == ']'
		;
}

void validate_id(const std::string& id);

}

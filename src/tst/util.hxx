#pragma once

#include <string>

#include <utki/debug.hpp>

namespace tst{

class check_failed{
public:
	const std::string file;
	const size_t line;
	const std::string message;

	check_failed(const std::string& message, const utki::source_location& source_location) :
			file(source_location.file_name()),
			line(source_location.line()),
			message(message)
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

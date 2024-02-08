/*
MIT License

Copyright (c) 2021-2023 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <string>

#include <utki/debug.hpp>

namespace tst {

class check_failed
{
public:
	const utki::source_location source_location;
	const std::string message;

	check_failed(std::string message, utki::source_location&& source_location) :
		source_location(std::move(source_location)),
		message(std::move(message))
	{}
};

struct full_id {
	const std::string& suite;
	const std::string& test;
};

inline bool is_valid_id_char(char c)
{
	return //
		('a' <= c && c <= 'z') || //
		('A' <= c && c <= 'Z') || //
		('0' <= c && c <= '9') || //
		c == '_' || //
		c == '[' || //
		c == ']';
}

void validate_id(std::string_view id);

void print_warning(std::ostream& o, const std::string& message);

} // namespace tst

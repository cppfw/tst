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

#include "util.hxx"

#include <algorithm>
#include <sstream>

#include "settings.hxx"

void tst::validate_id(std::string_view id)
{
	auto i = std::find_if(id.begin(), id.end(), [](std::remove_reference_t<decltype(id)>::value_type c) {
		return !is_valid_id_char(c);
	});
	if (i != id.end()) {
		std::stringstream ss;
		ss << "test case id validation failed: character '" << *i << "' is not allowed; id = " << id;
		throw std::invalid_argument(ss.str());
	}
}

void tst::print_warning(std::ostream& o, const std::string& message)
{
	std::stringstream ss;
	if (settings::inst().colored_output) {
		ss << "\033[1;35mwarning\033[0m: ";
	} else {
		ss << "warning: ";
	}
	ss << message << '\n';
	o << ss.str();
}

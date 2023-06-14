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

#include <utki/singleton.hpp>
#include <utki/util.hpp>

namespace tst {

class settings : public utki::intrusive_singleton<settings>
{
	friend singleton_type;

	// singleton instance variable
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
	static instance_type instance;

public:
	bool colored_output = utki::is_cout_terminal();

	bool show_help = false;

	bool list_tests = false;

	bool run_disabled = false;

	bool print_about_to_run = false;
	bool print_passed = false;
	bool print_skipped = false;
	bool print_outcome = false;

	unsigned long num_threads = 1;

	std::string junit_report_out_file;

	bool run_list_stdin = false;

	std::string suite_name;
	std::string test_name;
};

} // namespace tst

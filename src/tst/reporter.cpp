/*
MIT License

Copyright (c) 2021-2025 Ivan Gagis

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

#include "reporter.hxx"

#include <fstream>
#include <iostream>
#include <ratio>

#include "settings.hxx"

using namespace tst;

void reporter::report(const full_id& id, suite::status result, uint32_t dt, std::string message)
{
	std::lock_guard<decltype(this->mutex)> lock_guard(this->mutex);

	auto si = this->app.suites.find(id.suite);
	ASSERT(si != this->app.suites.end())

	auto& s = si->second;

	auto pi = s.tests.find(id.test);
	ASSERT(pi != s.tests.end())

	auto& info = pi->second;

	info.result = result;
	info.time_ms = dt;
	info.message = std::move(message);

	switch (result) {
		case decltype(result)::passed:
			++s.num_passed;
			++this->num_passed;
			break;
		case decltype(result)::failed:
			++s.num_failed;
			++this->num_failed;
			break;
		case decltype(result)::errored:
			++s.num_errors;
			++this->num_errors;
			break;
		case decltype(result)::disabled:
			++s.num_disabled;
			++this->num_disabled;
			break;
		default:
			break;
	}
}

void reporter::print_num_tests_about_to_run(std::ostream& o) const
{
	size_t actual_num = this->app.run_list_size();
	if (actual_num == 0) {
		actual_num = this->num_tests;
	}

	if (tst::settings::inst().colored_output) {
		o << "\033[1;33;4mrunning\033[0m ";
	} else {
		o << "running ";
	}
	o << actual_num << " test(s)";

	ASSERT(actual_num <= this->num_tests)

	if (actual_num != this->num_tests) {
		o << " out of " << this->num_tests;
	}
	o << std::endl;
}

void reporter::print_num_tests_run(std::ostream& o) const
{
	o << "ran " << this->num_ran() << " test(s) in " << this->time_ms << " ms" << '\n';
}

void reporter::print_num_tests_passed(std::ostream& o) const
{
	if (settings::inst().colored_output) {
		o << "\033[1;32m" << this->num_passed << "\033[0m";
	} else {
		o << this->num_passed;
	}
	o << " test(s) passed" << std::endl;
}

void reporter::print_num_tests_disabled(std::ostream& o) const
{
	if (this->num_disabled == 0) {
		return;
	}

	if (settings::inst().colored_output) {
		std::cout << "\033[0;33m" << this->num_disabled << "\033[0m";
	} else {
		std::cout << this->num_disabled;
	}
	std::cout << " test(s) disabled" << std::endl;
}

void reporter::print_num_tests_failed(std::ostream& o) const
{
	size_t num = this->num_unsuccessful();
	if (num == 0) {
		return;
	}

	if (settings::inst().colored_output) {
		std::cout << "\033[1;31m" << num << "\033[0m";
	} else {
		std::cout << num;
	}
	std::cout << " test(s) failed" << std::endl;
}

void reporter::print_num_tests_skipped(std::ostream& o) const
{
	size_t num = this->num_skipped();
	if (num == 0) {
		return;
	}

	if (settings::inst().colored_output) {
		std::cout << "\033[1;90m" << num << "\033[0m";
	} else {
		std::cout << num;
	}
	std::cout << " test(s) skipped" << std::endl;
}

void reporter::print_num_warnings(std::ostream& o) const
{
	if (app.num_warnings == 0) {
		return;
	}

	if (tst::settings::inst().colored_output) {
		o << "\033[1;35m" << app.num_warnings << "\033[0m";
	} else {
		o << app.num_warnings;
	}

	o << " warning(s)" << std::endl;
}

void reporter::print_outcome(std::ostream& o) const
{
	if (!settings::inst().print_outcome) {
		return;
	}
	if (this->is_failed()) {
		// print FAILED word
		if (tst::settings::inst().colored_output) {
			o << "\t\033[1;31mFAILED\033[0m" << std::endl;
		} else {
			o << "\tFAILED" << std::endl;
		}
	} else {
		// print PASSED word
		if (tst::settings::inst().colored_output) {
			o << "\t\033[1;32mPASSED\033[0m" << std::endl;
		} else {
			o << "\tPASSED" << std::endl;
		}
	}
}

// See https://llg.cubic.org/docs/junit/ for junit report format
void reporter::write_junit_report(const std::string& file_name) const
{
	std::ofstream f(file_name, std::ios::binary);

	f << R"(<?xml version="1.0" encoding="UTF-8"?>)" << '\n';
	f << "<testsuites"
		 " name='"
	  << this->app.name
	  << "'"
		 " tests='"
	  << this->num_tests
	  << "'"
		 " disabled='"
	  << this->num_disabled
	  << "'"
		 " errors='"
	  << this->num_errors
	  << "'"
		 " failures='"
	  << this->num_failed
	  << "'"
		 " skipped='"
	  << this->num_skipped()
	  << "'"
		 " time='"
	  << (double(this->time_ms) / std::milli::den) << "'>" << '\n';

	for (const auto& si : this->app.suites) {
		auto& s = si.second;
		f << "\t<testsuite"
			 " name='"
		  << si.first
		  << "'"
			 " tests='"
		  << s.size()
		  << "'"
			 " disabled='"
		  << s.num_disabled
		  << "'"
			 " failures='"
		  << s.num_failed
		  << "'"
			 " errors='"
		  << s.num_errors
		  << "'"
			 " skipped='"
		  << s.num_skipped()
		  << "'"
		  //" time='" << "TODO: set time in seconds" << '\''
		  << '>' << '\n';

		for (const auto& ti : s.tests) {
			auto& t = ti.second;

			f << "\t\t<testcase"
				 " name='"
			  << ti.first
			  << "'"
				 " status='"
			  << suite::status_to_string(t.result)
			  << "'"
				 " time='"
			  << (double(t.time_ms) / std::milli::den) << '\'';

			switch (t.result) {
				case suite::status::errored:
				case suite::status::failed:
				case suite::status::not_run:
					f << '>' << '\n';
					f << "\t\t\t<" << ([&]() {
						switch (t.result) {
							default:
								ASSERT(false)
							case suite::status::errored:
								return "error";
							case suite::status::failed:
								return "failure";
							case suite::status::not_run:
								return "skipped";
						}
					}())
					  << " message='" << t.message << "'/>" << '\n';
					f << "\t\t</testcase>";
					break;
				default:
					f << "/>";
			}

			f << '\n';
		}

		f << "\t</testsuite>" << '\n';
	}

	f << "</testsuites>" << '\n';
	f.flush();
}

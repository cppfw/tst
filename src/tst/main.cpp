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

#include <clargs/parser.hpp>
#include <utki/config.hpp>
#include <utki/util.hpp>

#include "application.hpp"
#include "settings.hxx"
#include "util.hxx"

namespace tst {

// unexpected exceptions are not caught
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(utki::span<const char*> args)
{
	settings settings_singleton;

	std::unique_ptr<application> app;

	auto& factory = application_factory::get_factory();
	if (factory) {
		app = factory();

		if (!app) {
			throw std::logic_error("tst::create_application() returned nullptr");
		}
	} else {
		LOG([](auto& o) {
			o << "tst::create_application() function not found, creating basic "
				 "application"
			  << '\n';
		})
		app = std::make_unique<application>();
	}

	app->cli.parse(args);

	if (settings::inst().show_help) {
		app->print_help();
		return 0;
	}

	app->init();

	if (settings::inst().list_tests) {
		app->list_tests(std::cout);
		return 0;
	}

	if (!settings::inst().suite_name.empty()) {
		app->set_run_list_from_suite_and_test_name();
	} else if (!settings::inst().test_name.empty()) {
		throw std::invalid_argument("--test argument requires --suite argument");
	} else if (settings::inst().run_list_stdin) {
		app->read_run_list_from_stdin();
	}

	return app->run();
}

} // namespace tst

// unexpected exceptions are not caught
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, const char** argv)
{
	return tst::main(utki::make_span(argv, argc));
}

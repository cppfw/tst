/*
MIT License

Copyright (c) 2021 Ivan Gagis

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

#include <memory>
#include <unordered_map>
#include <set>
#include <string_view>
#include <functional>

#include <utki/config.hpp>

#include <clargs/parser.hpp>

#include "suite.hpp"

namespace tst{

/**
 * @brief Test runner application.
 * The application class provides an entry point for the test program.
 * In case one needs more flexibility on how the test program is being started/finalized
 * then one can subclass the application class and override some of it's behaviour.
 * In that case, one also has to provide the tst::create_application() factory function
 * which will construct the application instance.
 */
class application{

	// NOLINTNEXTLINE(bugprone-exception-escape): unexpected exceptions are not caught
	friend int main(utki::span<const char*> args);

	friend class reporter;
	friend class iterator;

	const std::string name;
	const std::string description;

	std::unordered_map<std::string, suite> suites;

	std::unordered_map<std::string_view, std::set<std::string_view>> run_list;

	bool is_in_run_list(const std::string& suite, const std::string& test)const;

	void print_help()const;

	int run();

	size_t num_tests()const noexcept;

	size_t run_list_size()const noexcept;

	void list_tests(std::ostream& o)const;

	void read_run_list_from_stdin();
	void set_run_list_from_suite_and_test_name();

	size_t num_warnings = 0;
public:
	/**
	 * @brief Constructor.
	 * @param name - the test application name.
	 *               This name will be shown in the test application's help output.
	 * @param description - description of the test application.
	 *                      This description will be shown in the test application's help output.
	 */
	application(
			std::string&& name = std::string(),
			std::string&& description = std::string()
		);

	virtual ~application(){}

	/**
	 * @brief Initialize test cases.
	 * This function is called by tst right after command line arguments have been parsed.
	 * The default implementation of the function adds test cases to the application
	 * from all tst::set instances.
	 */
	virtual void init();

	/**
	 * @brief Command line arguments parser.
	 * This is the instance of the command line arguments parser.
	 * Command line arguments parsing is invoked by tst right after the application instance is created.
	 * So, in case one needs to add some custom command line arguments to the parser,
	 * then one can do it from within the constructor of the application subclass.
	 */
	clargs::parser cli;

	/**
	 * @brief Get test suite.
	 * This function tries to search for the test suite with the given id.
	 * In case the suite is not found, it will be created.
	 * @param id - the id of the test suite to get.
	 * @return reference to the test suite with the requested id.
	 */
	suite& get_suite(const std::string& id);
};

/**
 * @brief Application factory registerer.
 * The object of this class registers the application factory function.
 * The application object will be constructed using the provided factory function at program start.
 */
class application_factory{

	// NOLINTNEXTLINE(bugprone-exception-escape): unexpected exceptions are not caught
	friend int main(utki::span<const char*> args);

public:
	typedef std::function<std::unique_ptr<application>()> factory_type;

	/**
	 * @brief Constructor.
	 * Registers the application object factory function.
	 * Only one application factory can be registered.
	 * @param factory - application factory function.
	 * @throw std::logic_error - in case a factory is already registered.
	 */
	application_factory(factory_type&& factory);
private:
	static factory_type& get_factory();
};

}

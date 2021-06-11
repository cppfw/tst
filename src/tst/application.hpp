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

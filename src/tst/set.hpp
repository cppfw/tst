#pragma once

#include <map>
#include <vector>
#include <functional>

#include "suite.hpp"
namespace tst{

/**
 * @brief Test case set.
 * This class provides a way to declare a group of test cases.
 * Just define an instance of the tst::set at a global scope and provide a test
 * case initializer function to its constructor.
 */
class set{
	friend class application;
	
	typedef std::map<std::string, std::vector<std::function<void(suite&)>>> inits_type;

	// The static inits map object has to be returned by getter function to avoid
	// undetermined initialization order of global variables problem.
	static inits_type& get_inits();
public:
	/**
	 * @brief Constructor.
	 * @param suite_name - name of the test suite to add the test cases to.
	 * @param init - initializer function which adds test cases to the test suite instance.
	 *               The function takes a reference to the test suite instance as argument.
	 */
	set(const std::string& suite_name, inits_type::value_type::second_type::value_type&& init);
};

}

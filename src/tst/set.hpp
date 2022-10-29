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
	
	using inits_type = std::map<std::string, std::vector<std::function<void(suite&)>>>;

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

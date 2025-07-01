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

#pragma once

#include <functional>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <utki/debug.hpp>
#include <utki/flags.hpp>

namespace tst {

enum class flag {
	disabled,
	no_parallel,

	enum_size
};

/**
 * @brief Test suite.
 * The test suite object holds test case definitions belonging to a particular
 * test suite.
 */
class suite
{
	friend class application;
	friend class reporter;
	friend class iterator;

	enum class status {
		not_run,
		passed,
		failed,
		errored,
		disabled
	};

	static const char* status_to_string(status s);

	// TODO: why lint complains?
	// "error: an exception may be thrown in function 'test_info'"
	// NOLINTNEXTLINE(bugprone-exception-escape)
	struct test_info {
		std::function<void()> proc;
		utki::flags<flag> flags;
		mutable status result = status::not_run;
		mutable uint32_t time_ms;
		mutable std::string message;
	};

	std::unordered_map<std::string, test_info> tests;

	suite() = default;

	mutable size_t num_disabled = 0;
	mutable size_t num_failed = 0;
	mutable size_t num_passed = 0;
	mutable size_t num_errors = 0;

	size_t num_skipped() const noexcept
	{
		size_t num_non_skipped = this->num_disabled + this->num_errors + this->num_failed + this->num_passed;
		ASSERT(this->tests.size() >= num_non_skipped)
		return tests.size() - num_non_skipped;
	}

	static std::string make_indexed_id(std::string_view id, size_t index);

public:
	// TODO: is it possible to hide the move constructor from user?
	suite(suite&&) = default;
	suite& operator=(suite&&) = default;

	suite(const suite&) = delete;
	suite& operator=(const suite&) = delete;

	~suite() = default;

	/**
	 * @brief Get number of test cases in the test suite.
	 */
	size_t size() const noexcept
	{
		return this->tests.size();
	}

	/**
	 * @brief Add a simple test case to the test suite.
	 * @param id - id of the test case.
	 * @param flags - test marks.
	 * @param proc - test case procedure.
	 */
	void add(std::string id, utki::flags<flag> flags, std::function<void()> proc);

	/**
	 * @brief Add a simple test case to the test suite.
	 * @param id - id of the test case.
	 * @param proc - test case procedure.
	 */
	void add(std::string id, std::function<void()> proc)
	{
		this->add(std::move(id), false, std::move(proc));
	}

	/**
	 * @brief Add a simple disabled test case to the test suite.
	 * This method is same as corresponding 'add()' method but it
	 * implicitly adds a 'disabled' mark to the test case.
	 * @param id - id of the test case.
	 * @param flags - test marks.
	 * @param proc - test case procedure.
	 */
	void add_disabled(std::string id, utki::flags<flag> flags, std::function<void()> proc);

	/**
	 * @brief Add a simple disabled test case to the test suite.
	 * This method is same as corresponding 'add()' method but it
	 * implicitly adds a 'disabled' mark to the test case.
	 * @param id - id of the test case.
	 * @param proc - test case procedure.
	 */
	void add_disabled(std::string id, std::function<void()> proc)
	{
		this->add_disabled(std::move(id), false, std::move(proc));
	}

	/**
	 * @brief Add parametrized test case to the test suite.
	 * For each parameter value, adds a test case to the suite.
	 * The actual test case ids are composed of the provided id string and
	 * '[index]' suffix where index is the index of the parameter.
	 * @param id - id of the test case.
	 * @param flags - test marks.
	 * @param params - collection of test procedure parameters.
	 * @param proc - test procedure which takes a const reference to a parameter
	 * as argument.
	 */
	template <class parameter_type>
	void add(
		std::string id,
		utki::flags<flag> flags,
		std::vector<parameter_type> params,
		std::function<void(const parameter_type&)> proc
	)
	{
		auto shared_proc = std::make_shared<std::function<void(const parameter_type&)>>(std::move(proc));
		for (size_t i = 0; i != params.size(); ++i) {
			// TODO: why lint complains here on macos?
			// "error: an exception may be thrown"
			// NOLINTNEXTLINE(bugprone-exception-escape)
			this->add(make_indexed_id(id, i), flags, [proc = shared_proc, param = std::move(params[i])]() {
				ASSERT(proc)
				ASSERT(*proc)
				(*proc)(param);
			});
		}
	}

	/**
	 * @brief Add parametrized test case to the test suite.
	 * For each parameter value, adds a test case to the suite.
	 * The actual test case ids are composed of the provided id string and
	 * '[index]' suffix where index is the index of the parameter.
	 * @param id - id of the test case.
	 * @param params - collection of test procedure parameters.
	 * @param proc - test procedure which takes a const reference to a parameter
	 * as argument.
	 */
	template <class parameter_type>
	void add(
		std::string id, //
		std::vector<parameter_type> params,
		std::function<void(const parameter_type&)> proc
	)
	{
		this->add(std::move(id), false, std::move(params), std::move(proc));
	}

	/**
	 * @brief Add disabled parametrized test case to the test suite.
	 * For each parameter value, adds a test case to the suite.
	 * The actual test case ids are composed of the provided id string and
	 * '[index]' suffix where index is the index of the parameter. Note, that
	 * parameter type has to be indicated as a template argument of the function.
	 * This method is same as corresponding 'add()' method but it
	 * implicitly adds a 'disabled' mark to the test case.
	 * @param id - id of the test case.
	 * @param flags - test marks.
	 * @param params - collection of test procedure parameters.
	 * @param proc - test procedure which takes a const reference to a parameter
	 * as argument.
	 */
	template <class parameter_type>
	void add_disabled(
		std::string id,
		utki::flags<flag> flags,
		std::vector<parameter_type> params,
		std::function<void(const parameter_type&)> proc
	)
	{
		flags.set(flag::disabled);
		this->add(std::move(id), flags, std::move(params), std::move(proc));
	}

	/**
	 * @brief Add disabled parametrized test case to the test suite.
	 * For each parameter value, adds a test case to the suite.
	 * The actual test case ids are composed of the provided id string and
	 * '[index]' suffix where index is the index of the parameter. Note, that
	 * parameter type has to be indicated as a template argument of the function.
	 * This method is same as corresponding 'add()' method but it
	 * implicitly adds a 'disabled' mark to the test case.
	 * @param id - id of the test case.
	 * @param params - collection of test procedure parameters.
	 * @param proc - test procedure which takes a const reference to a parameter
	 * as argument.
	 */
	template <class parameter_type>
	void add_disabled(
		std::string id,
		std::vector<parameter_type> params,
		std::function<void(const parameter_type&)> proc
	)
	{
		this->add_disabled(std::move(id), false, std::move(params), std::move(proc));
	}
};

} // namespace tst

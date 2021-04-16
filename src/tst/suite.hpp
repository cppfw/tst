#pragma once

#include <unordered_map>
#include <vector>
#include <sstream>
#include <functional>

#include <utki/debug.hpp>

namespace tst{

/**
 * @brief Test suite.
 * The test suite object holds test case definitions belonging to a particular test suite.
 */
class suite{
	friend class application;
	friend class reporter;
	friend class iterator;

	enum status{
		not_run,
		passed,
		failed,
		errored,
		disabled
	};

	static const char* status_to_string(status s);

	struct test_info{
		std::function<void()> proc;
		mutable status result = status::not_run;
		mutable uint32_t time_ms;
		mutable std::string message;
	};

	std::unordered_map<std::string, test_info> tests;

	suite(){}

	mutable size_t num_disabled = 0;
	mutable size_t num_failed = 0;
	mutable size_t num_passed = 0;
	mutable size_t num_errors = 0;

	size_t num_skipped()const noexcept{
		size_t num_non_skipped = this->num_disabled + this->num_errors + this->num_failed + this->num_passed;
		ASSERT(this->tests.size() >= num_non_skipped)
		return tests.size() - num_non_skipped;
	}

	void add_disabled(const std::string& id);

	void add_disabled(const std::string& id, size_t num_tests);

	static std::string make_indexed_id(const std::string& id, size_t index);
public:
	// TODO: is it possible to hide the move constructor from user?
	suite(suite&&) = default;

	suite(const suite&) = delete;
	suite& operator=(const suite&) = delete;

	/**
	 * @brief Get number of test cases in the test suite.
	 */
	size_t size()const noexcept{
		return this->tests.size();
	}

	/**
	 * @brief Add a simple test case to the test suite.
	 * @param id - id of the test case.
	 * @param proc - test case procedure.
	 */
	void add(const std::string& id, std::function<void()>&& proc);

	/**
	 * @brief Add a simple disabled test case to the test suite.
	 * @param id - id of the test case.
	 * @param proc - test case procedure.
	 */
	void add_disabled(const std::string& id, std::function<void()>&& proc);

	/**
	 * @brief Add parametrized test case to the test suite.
	 * For each parameter value, adds a test case to the suite.
	 * The actual test case ids are composed of the provided id string and '[index]' suffix where index is the
	 * index of the parameter.
	 * @param id - id of the test case.
	 * @param params - collection of test procedure parameters.
	 * @param proc - test procedure which takes a const reference to a parameter as argument.
	 */
	template <class parameter>
	void add(const std::string& id, std::vector<parameter>&& params, const std::function<void(const parameter&)>& proc){
		for(size_t i = 0; i != params.size(); ++i){
			this->add(
					make_indexed_id(id, i),
					[proc = proc, param = std::move(params[i])](){
						ASSERT(proc != nullptr)
						proc(param);
					}
				);
		}
	}

	/**
	 * @brief Add disabled parametrized test case to the test suite.
	 * For each parameter value, adds a test case to the suite.
	 * The actual test case ids are composed of the provided id string and '[index]' suffix where index is the
	 * index of the parameter.
	 * Note, that parameter type has to be indicated as a template argument of the function.
	 * @param id - id of the test case.
	 * @param params - collection of test procedure parameters.
	 * @param proc - test procedure which takes a const reference to a parameter as argument.
	 */
	template <class parameter>
	void add_disabled(const std::string& id, std::vector<parameter>&& params, const std::function<void(const parameter&)>& proc){
		this->add_disabled(id, params.size());
	}
};

}

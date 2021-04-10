#pragma once

#include <string>
#include <map>
#include <mutex>

#include "tester.hpp"
#include "suite.hpp"

#include "full_id.hxx"

namespace tst{
class reporter{
private:
	std::mutex mutex;
	const decltype(tester::suites)& suites;

	uint32_t num_failed = 0;
	uint32_t num_passed = 0;
	uint32_t num_disabled = 0;

	// thread safe
	void report(
			const full_id& id,
			suite::status result,
			std::string&& error_message = std::string()
		);
public:
	reporter(decltype(suites)& suites) :
			suites(suites)
	{}
	
	// thread safe
	void report_pass(const full_id& id)
	{
		this->report(id, suite::status::passed);
	}

	// thread safe
	void report_failure(
			const full_id& id,
			std::string&& error_message
		)
	{
		this->report(id, suite::status::failed, std::move(error_message));
	}

	void report_disabled_test(){
		++this->num_disabled;
	}

	void print_num_tests_passed(std::ostream& o)const;
	void print_num_tests_disabled(std::ostream& o)const;
	void print_num_tests_failed(std::ostream& o)const;
	void print_outcome(std::ostream& o)const;

	bool is_failed()const noexcept{
		return this->num_failed != 0;
	}

	void write_junit_report(const std::string& file_name)const;
};

}

#pragma once

#include <string>
#include <mutex>

#include "suite.hpp"
#include "application.hpp"

#include "util.hxx"

namespace tst{
class reporter{
private:
	std::mutex mutex;
	const application& app;

	const size_t num_tests;
	
	size_t num_failed = 0;
	size_t num_passed = 0;
	size_t num_disabled = 0;
	size_t num_errors = 0;

	// thread safe
	void report(
			const full_id& id,
			suite::status result,
			std::string&& message = std::string()
		);
public:
	reporter(const application& app) :
			app(app),
			num_tests(app.num_tests())
	{}
	
	// thread safe
	void report_pass(const full_id& id)
	{
		this->report(id, suite::status::passed);
	}

	// thread safe
	void report_failure(
			const full_id& id,
			std::string&& message
		)
	{
		this->report(id, suite::status::failed, std::move(message));
	}

	// thread safe
	void report_error(
			const full_id& id,
			std::string&& message
		)
	{
		this->report(id, suite::status::errored, std::move(message));
	}

	// thread safe
	void report_disabled_test(const full_id& id){
		this->report(id, suite::status::disabled);
	}

	void print_num_tests_about_to_run(std::ostream& o)const;
	void print_num_tests_passed(std::ostream& o)const;
	void print_num_tests_disabled(std::ostream& o)const;
	void print_num_tests_failed(std::ostream& o)const;
	void print_outcome(std::ostream& o)const;

	bool is_failed()const noexcept{
		return this->num_failed != 0 || this->num_errors != 0;
	}

	void write_junit_report(const std::string& file_name)const;
};

}

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
			uint32_t dt,
			std::string&& message = std::string()
		);
public:
	uint32_t time_ms = 0;
	
	reporter(const application& app) :
			app(app),
			num_tests(app.num_tests())
	{}
	
	// thread safe
	void report_pass(const full_id& id, uint32_t dt)
	{
		this->report(id, suite::status::passed, dt);
	}

	// thread safe
	void report_failure(
			const full_id& id,
			uint32_t dt,
			std::string&& message
		)
	{
		this->report(id, suite::status::failed, dt, std::move(message));
	}

	// thread safe
	void report_error(
			const full_id& id,
			uint32_t dt,
			std::string&& message
		)
	{
		this->report(id, suite::status::errored, dt, std::move(message));
	}

	// thread safe
	void report_skipped(
			const full_id& id,
			std::string&& message
		)
	{
		this->report(id, suite::status::not_run, 0, std::move(message));
	}

	// thread safe
	void report_disabled_test(const full_id& id){
		this->report(id, suite::status::disabled, 0);
	}

	size_t num_unsuccessful()const noexcept{
		return this->num_failed + this->num_errors;
	}

	size_t num_not_run()const noexcept{
		return this->num_disabled + this->num_skipped();
	}

	size_t num_ran()const noexcept{
		return this->num_unsuccessful() + this->num_passed + this->num_disabled;
	}

	size_t num_skipped()const noexcept{
		ASSERT(this->num_tests >= this->num_ran())
		return this->num_tests - this->num_ran();
	}

	void print_num_tests_run(std::ostream& o)const;
	void print_num_tests_about_to_run(std::ostream& o)const;
	void print_num_tests_passed(std::ostream& o)const;
	void print_num_tests_disabled(std::ostream& o)const;
	void print_num_tests_failed(std::ostream& o)const;
	void print_num_tests_skipped(std::ostream& o)const;
	void print_num_warnings(std::ostream& o)const;
	void print_outcome(std::ostream& o)const;

	bool is_failed()const noexcept{
		return this->num_failed != 0 || this->num_errors != 0;
	}

	bool is_semi_passed()const noexcept{
		return !this->is_failed() && this->num_skipped() == 0 && this->num_not_run() == 0;
	}

	void write_junit_report(const std::string& file_name)const;
};

}

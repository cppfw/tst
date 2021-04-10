#pragma once

#include <map>

#include <clargs/parser.hpp>

#include "suite.hpp"

namespace tst{

class tester{
	friend int main(utki::span<const char*> args);

	tester(){}

	std::map<std::string, suite> suites;

	int run();

	uint32_t num_failed = 0;
	uint32_t num_passed = 0;
	uint32_t num_disabled = 0;

	void print_num_tests_about_to_run(std::ostream& o)const;
	void print_num_tests_passed(std::ostream& o)const;
	void print_num_tests_disabled(std::ostream& o)const;
	void print_num_tests_failed(std::ostream& o)const;
	void print_outcome(std::ostream& o)const;
public:
	std::string description;

	clargs::parser cli;

	suite& create_suite(const std::string& id);

	size_t size()const noexcept;

	bool is_failed()const noexcept{
		return this->num_failed != 0;
	}
};

}

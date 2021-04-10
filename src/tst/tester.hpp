#pragma once

#include <map>

#include <clargs/parser.hpp>

#include "suite.hpp"

namespace tst{

class tester{
	friend class iterator;
	friend class reporter;
	friend int main(utki::span<const char*> args);

	tester(){}

	std::map<std::string, suite> suites;

	int run();

	void print_num_tests_about_to_run(std::ostream& o)const;
public:
	std::string description;

	clargs::parser cli;

	suite& create_suite(const std::string& id);

	size_t size()const noexcept;
};

}

#pragma once

#include <map>

#include <clargs/parser.hpp>

#include "suite.hpp"

namespace testy{

class tester{
	friend int main(utki::span<const char*> args);

	tester(){}

	std::map<std::string, suite> suites;

	void run();

	unsigned num_failed = 0;
	unsigned num_passed = 0;
public:
	clargs::parser cli;
	std::function<void(std::string&&)> non_key_cli_handler;

	suite& create_suite(const std::string& id);
};

}

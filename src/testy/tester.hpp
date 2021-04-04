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

	uint32_t num_failed = 0;
	uint32_t num_passed = 0;
	uint32_t num_disabled = 0;
public:
	clargs::parser cli;
	std::function<void(std::string&&)> non_key_cli_handler;

	suite& create_suite(const std::string& id);
};

}

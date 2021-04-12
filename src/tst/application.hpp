#pragma once

#include <memory>
#include <map>
#include <set>
#include <string_view>

#include <utki/config.hpp>

#include <clargs/parser.hpp>

#include "suite.hpp"

namespace tst{

class application{
	friend int main(utki::span<const char*> args);
	friend class reporter;
	friend class iterator;

	const std::string name;
	const std::string description;

	std::map<std::string, suite> suites;

	std::map<std::string_view, std::set<std::string_view>> run_list;

	void print_help()const;

	int run();

	size_t num_tests()const noexcept;

	void list_tests(std::ostream& o)const;

	void read_run_list_from_stdin();
public:
	application(
			std::string&& name,
			std::string&& description
		);

	virtual ~application(){}

	virtual void init(){}

	clargs::parser cli;

	suite& create_suite(const std::string& id);
};

#if M_OS == M_OS_WINDOWS
__declspec(dllexport)
#endif
std::unique_ptr<tst::application> create_application();

}

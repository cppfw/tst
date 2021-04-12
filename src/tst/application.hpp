#pragma once

#include <memory>

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

	void print_help()const;

	int run();

	size_t num_tests()const noexcept;

	void list_tests(std::ostream& o)const;

protected:
	clargs::parser cli;

	suite& create_suite(const std::string& id);

public:
	application(
			std::string&& name,
			std::string&& description
		);

	virtual ~application(){}

	virtual bool init() = 0;
};

#if M_OS == M_OS_WINDOWS
__declspec(dllexport)
#endif
std::unique_ptr<tst::application> create_application();

}

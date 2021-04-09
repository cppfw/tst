#pragma once

#include <map>

#include <clargs/parser.hpp>
#include <nitki/thread.hpp>
#include <nitki/queue.hpp>

#include "suite.hpp"

namespace tst{

class tester{
	friend int main(utki::span<const char*> args);

	tester(){}

	std::map<std::string, suite> suites;

	nitki::queue queue;

	void run();

	uint32_t num_failed = 0;
	uint32_t num_passed = 0;
	uint32_t num_disabled = 0;
public:
	std::string description;

	clargs::parser cli;

	suite& create_suite(const std::string& id);

	size_t size()const noexcept;

private:
	struct runner : public nitki::thread{
		tester& owner;

		runner(tester& owner) : owner(owner){}

		nitki::queue queue;

		void run()override;
	};
};

}

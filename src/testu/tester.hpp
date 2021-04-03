#pragma once

#include <functional>
#include <map>
#include <vector>
#include <sstream>

#include <utki/span.hpp>
#include <clargs/parser.hpp>

namespace testu{

class tester{
	friend int main(utki::span<const char*> args);

	tester(){}

	std::map<std::string, std::function<void()>> procedures;

	void run();

	unsigned num_failed = 0;
	unsigned num_passed = 0;
public:
	clargs::parser cli;
	std::function<void(std::string&&)> non_key_cli_handler;

	void add(const std::string& id, std::function<void()>&& proc);

	template <class fixture>
	void add(const std::string& id, std::function<void(fixture&)>&& proc){
		this->add(
				id,
				[proc = std::move(proc)](){
					fixture f;
					proc(f);
				}
			);
	}

	template <class Param>
	void add(const std::string& id, std::vector<Param>&& params, const std::function<void(const Param&)>& proc){
		for(size_t i = 0; i != params.size(); ++i){
			std::stringstream ss;
			ss << id << " [" << i << "]";
			this->add(
					ss.str(),
					[proc = proc, param = std::move(params[i])](){
						ASSERT(proc)
						proc(param);
					}
				);
		}
	}
	// TODO: parametrized fixtured tests

	// TODO: persistent fixtures

	// TODO: disabled tests
};

}

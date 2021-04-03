#pragma once

#include <map>
#include <vector>
#include <sstream>
#include <functional>

#include <utki/debug.hpp>

namespace testy{

class suite{
	friend class tester;

	std::map<std::string, std::function<void()>> procedures;

	suite(){}

public:
	suite(suite&&) = default;

	suite(const suite&) = delete;
	suite& operator=(const suite&) = delete;

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
			ss << id << " (" << i << ")";
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

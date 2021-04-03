#include "suite.hpp"

using namespace testy;

void suite::add(const std::string& id, std::function<void()>&& proc){
	auto r = this->procedures.insert(std::make_pair(id, std::move(proc)));
	if(!r.second){
		std::stringstream ss;
		ss << "test with id = '" << id << "' already exists in the test suite";
		throw std::invalid_argument(ss.str());
	}
}

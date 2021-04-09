#include "suite.hpp"

#include "util.hxx"

using namespace tst;

void suite::add(const std::string& id, std::function<void()>&& proc){
	validate_id(id);

	auto r = this->procedures.insert(std::make_pair(id, std::move(proc)));
	if(!r.second){
		std::stringstream ss;
		ss << "test with id = '" << id << "' already exists in the test suite";
		throw std::invalid_argument(ss.str());
	}
}

void suite::add_disabled(const std::string& id, size_t num_tests){
	if(num_tests == 0){
		this->add(id, nullptr);
	}else{
		for(size_t i = 0; i != num_tests; ++i){
			std::stringstream ss;
			ss << id << "[" << i << "]";
			this->add(ss.str(), nullptr);
		}
	}
}

void suite::add_disabled(const std::string& id, std::function<void()>&& proc){
	this->add_disabled(id);
}

#include "suite.hpp"

#include "util.hxx"

using namespace tst;

void suite::add(const std::string& id, std::function<void()>&& proc){
	validate_id(id);

	auto r = this->tests.insert(std::make_pair(id, test_info{std::move(proc)}));
	if(!r.second){
		std::stringstream ss;
		ss << "test with id = '" << id << "' already exists in the test suite";
		throw std::invalid_argument(ss.str());
	}
	auto& info = r.first->second;
	if(!info.proc){
		info.result = status::disabled;
		++this->num_disabled;
	}
}

void suite::add_disabled(const std::string& id){
	this->add(id, nullptr);
}

void suite::add_disabled(const std::string& id, size_t num_tests){
	for(size_t i = 0; i != num_tests; ++i){
		std::stringstream ss;
		ss << id << "[" << i << "]";
		this->add_disabled(ss.str());
	}
}

void suite::add_disabled(const std::string& id, std::function<void()>&& proc){
	this->add_disabled(id);
}

const char* suite::status_to_string(status s){
	switch(s){
		case status::passed:
			return "passed";
		case status::disabled:
			return "disabled";
		case status::errored:
			return "errored";
		case status::failed:
			return "failed";
		case status::not_run:
			return "not run";
	}

	ASSERT(false)
	return nullptr;
}

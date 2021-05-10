#include "suite.hpp"

#include "util.hxx"
#include "settings.hxx"

using namespace tst;

void suite::add(const std::string& id, std::function<void()>&& proc){
	validate_id(id);

	auto r = this->tests.insert(std::make_pair(id, test_info{std::move(proc)}));
	if(!r.second){
		std::stringstream ss;
		ss << "test with id = '" << id << "' already exists in the test suite";
		throw std::invalid_argument(ss.str());
	}
}

void suite::add_disabled(const std::string& id){
	this->add(id, nullptr);
}

void suite::add_disabled(const std::string& id, std::function<void()>&& proc){
	if(settings::inst().run_disabled){
		this->add(id, std::move(proc));
	}else{
		this->add_disabled(id);
	}
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

std::string suite::make_indexed_id(const std::string& id, size_t index){
	std::stringstream ss;
	ss << id << "[" << index << "]";
	return ss.str();
}

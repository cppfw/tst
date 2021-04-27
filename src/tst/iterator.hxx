#pragma once

#include "application.hpp"

#include "util.hxx"

namespace tst{

class iterator{
	const decltype(application::suites)& suites;

	decltype(application::suites)::const_iterator si;
	decltype(suite::tests)::const_iterator pi;

	void init_pi(){
		for(;this->si != this->suites.end(); ++this->si){
			if(!this->si->second.tests.empty()){
				this->pi = this->si->second.tests.begin();
				return;
			}
		}
	}
public:
	iterator(decltype(suites)& suites) :
			suites(suites),
			si(suites.begin())
	{
		this->init_pi();
	}

	bool is_valid()const{
		return this->si != this->suites.end();
	}

	void next(){
		ASSERT(this->is_valid())
		++this->pi;
		if(this->pi == this->si->second.tests.end()){
			++this->si;
			this->init_pi();
		}
	}

	const suite::test_info& info()const{
		ASSERT(this->is_valid())
		return this->pi->second;
	}

	full_id id()const{
		ASSERT(this->is_valid())
		return {this->si->first, this->pi->first};
	}
};

}

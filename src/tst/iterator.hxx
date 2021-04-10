#pragma once

#include "tester.hpp"

#include "full_id.hxx"

namespace tst{

class iterator{
	const decltype(tester::suites)& suites;

	decltype(tester::suites)::const_iterator si;
	decltype(suite::tests)::const_iterator pi;
public:
	iterator(decltype(suites)& suites) :
			suites(suites),
			si(suites.begin())
	{
		if(this->is_valid())
			this->pi = this->si->second.tests.begin();
	}

	bool is_valid()const{
		return this->si != this->suites.end();
	}

	void next(){
		ASSERT(this->is_valid())
		++this->pi;
		if(this->pi == this->si->second.tests.end()){
			++this->si;
			if(this->si != this->suites.end()){
				this->pi = this->si->second.tests.begin();
			}
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

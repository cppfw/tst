#include "tester.hpp"

#include <iostream>
#include <sstream>

#include <utki/debug.hpp>

#include "check_failed.hxx"

using namespace testu;

void tester::add(const std::string& id, std::function<void()>&& proc){
	auto r = this->procedures.insert(std::make_pair(id, std::move(proc)));
	if(!r.second){
		std::stringstream ss;
		ss << "test with id = " << id << " is already added";
		throw std::invalid_argument(ss.str());
	}
}

void tester::run(){
	// TODO: parallel run
	for(auto& p : this->procedures){
		std::cout << "\e[1;33;4mrun\e[0m " << p.first << std::endl;
		try{
			ASSERT(p.second)
			p.second();
			++this->num_passed;
		}catch(testu::check_failed& e){
			++this->num_failed;
			std::stringstream ss;
			ss << "\e[1;31mfailed\e[0m: " << p.first;
			std::cout << ss.str() << std::endl;
		}
	}
}

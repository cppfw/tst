#include "tester.hpp"

#include <iostream>
#include <sstream>

#include <utki/debug.hpp>

#include "util.hxx"
#include "settings.hxx"

using namespace testy;

size_t tester::size()const noexcept{
	size_t ret = 0;

	for(const auto& s : this->suites){
		ret += s.second.size();
	}

	return ret;
}

void tester::run(){
	// TODO: parallel run
	for(const auto& s : this->suites){
		for(const auto& p : s.second.procedures){
			try{
				if(p.second){
					// print name of the test about to run
					if(settings::inst().is_cout_terminal){
						std::cout << "\e[1;33mrun\e[0m ";
					}else{
						std::cout << "run ";
					}
					std::cout << s.first << ": " << p.first << std::endl;

					p.second();
					++this->num_passed;
				}else{
					// print name of the disabled test
					if(settings::inst().is_cout_terminal){
						std::cout << "\e[0;33mdisabled\e[0m ";
					}else{
						std::cout << "disabled ";
					}
					std::cout << s.first << ": " << p.first << std::endl;

					++this->num_disabled;
				}
			}catch(testy::check_failed& e){
				++this->num_failed;

				// use stringstream to make all info printed without interruption in case of parallel tests running
				std::stringstream ss;

				// print name of the failed test
				if(settings::inst().is_cout_terminal){
					ss << "\e[1;31mfailed\e[0m: ";
				}else{
					ss << "failed: ";
				}
				ss << s.first << "/" << p.first << std::endl;

				// print error information
				ss << "  " << e.file << ":" << e.line;
				if(settings::inst().is_cout_terminal){
					ss << ": \e[1;31merror\e[0m: ";
				}else{
					ss << ": error: ";
				}
				ss << e.message;

				std::cout << ss.str() << std::endl;
			}
		}
	}
}

suite& tester::create_suite(const std::string& id){
	validate_id(id);

	auto i = this->suites.emplace(id, suite());
	if(!i.second){
		std::stringstream ss;
		ss << "could not create test suite: suite with id '" << id << "' already exists";
		throw std::invalid_argument(ss.str());
	}
	return i.first->second;
}

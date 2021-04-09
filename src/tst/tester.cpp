#include "tester.hpp"

#include <iostream>
#include <sstream>

#include <utki/debug.hpp>

#include "util.hxx"
#include "settings.hxx"

using namespace tst;

size_t tester::size()const noexcept{
	size_t ret = 0;

	for(const auto& s : this->suites){
		ret += s.second.size();
	}

	return ret;
}

void tester::run(){
	// TODO: parallel run (check settings::num_threads)
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
			}catch(tst::check_failed& e){
				++this->num_failed;

				// use stringstream to make all info printed without interruption in case of parallel tests running
				std::stringstream ss;

				// print name of the failed test
				if(settings::inst().is_cout_terminal){
					ss << "\e[1;31mfailed\e[0m: ";
				}else{
					ss << "failed: ";
				}
				ss << s.first << ": " << p.first << std::endl;

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

void tester::runner::run(){
	// TODO:
}

void tester::print_num_tests_about_to_run(std::ostream& o)const{
	if(tst::settings::inst().is_cout_terminal){
		o << "\e[1;33;4mrunning\e[0m ";
	}else{
		o << "running ";
	}
	o << this->size() << " test(s)" << std::endl;
}

void tester::print_num_tests_passed(std::ostream& o)const{
	if(settings::inst().is_cout_terminal){
		o << "\e[1;32m" << this->num_passed << "\e[0m";
	}else{
		o << this->num_passed;
	} 
	o << " test(s) passed" << std::endl;
}

void tester::print_num_tests_disabled(std::ostream& o)const{
	if(this->num_disabled == 0){
		return;
	}

	if(settings::inst().is_cout_terminal){
		std::cout << "\e[0;33m" << this->num_disabled << "\e[0m";
	}else{
		std::cout << this->num_disabled;
	}
	std::cout << " test(s) disabled" << std::endl;
}

void tester::print_num_tests_failed(std::ostream& o)const{
	if(this->num_failed == 0){
		return;
	}

	if(settings::inst().is_cout_terminal){
		std::cout << "\e[1;31m" << this->num_failed  << "\e[0m";
	}else{
		std::cout << this->num_failed;
	}
	std::cout << " test(s) failed" << std::endl;
}

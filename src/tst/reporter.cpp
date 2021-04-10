#include "reporter.hxx"

#include <fstream>

#include "settings.hxx"

using namespace tst;

void reporter::report(
		const full_id& id,
		suite::status result,
		std::string&& error_message
	)
{
	std::lock_guard<decltype(this->mutex)> lock_guard(this->mutex);

	auto si = this->suites.find(id.suite);
	ASSERT(si != this->suites.end())

	auto& s = si->second;

	auto pi = s.tests.find(id.test);
	ASSERT(pi != s.tests.end())

	auto& info = pi->second;

	info.result = result;
	info.error_message = std::move(error_message);

	switch(result){
		case decltype(result)::passed:
			++s.num_passed_tests;
			++this->num_passed;
			break;
		case decltype(result)::failed:
			++s.num_failed_tests;
			++this->num_failed;
			break;
		default:
			break;
	}
}

void reporter::print_num_tests_passed(std::ostream& o)const{
	if(settings::inst().is_cout_terminal){
		o << "\e[1;32m" << this->num_passed << "\e[0m";
	}else{
		o << this->num_passed;
	} 
	o << " test(s) passed" << std::endl;
}

void reporter::print_num_tests_disabled(std::ostream& o)const{
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

void reporter::print_num_tests_failed(std::ostream& o)const{
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

void reporter::print_outcome(std::ostream& o)const{
	if(this->is_failed()){
		// print FAILED word
		if(tst::settings::inst().is_cout_terminal){
			o << "\t\e[1;31mFAILED\e[0m" << std::endl;
		}else{
			o << "\tFAILED" << std::endl;
		}
	}else{
		// print PASSED word
		if(tst::settings::inst().is_cout_terminal){
			o << "\t\e[1;32mPASSED\e[0m" << std::endl;
		}else{
			o << "\tPASSED" << std::endl;
		}
	}
}

void reporter::write_junit_report(const std::string& file_name)const{
	std::ofstream f(file_name, std::ios::binary);

	for(const auto& s : this->suites){
		for(const auto& t : s.second.tests){
			auto& i = t.second;
			// TODO:
			f << i.error_message << std::endl;
		}
	}
}

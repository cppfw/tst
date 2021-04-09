#include "tester.hpp"

#include <iostream>
#include <sstream>

#include <utki/debug.hpp>
#include <nitki/thread.hpp>
#include <nitki/queue.hpp>

#include "util.hxx"
#include "settings.hxx"

using namespace tst;

namespace{
void print_test_name(std::ostream& o, const std::string& suite, const std::string& test){
	if(settings::inst().is_cout_terminal){
		o << "\e[1;90m(" << suite << ")\e[0m \e[0;36m" << test << "\e[0m" << std::endl;
	}else{
		o << "(" << suite << ") " << test << std::endl;
	}
}
}

namespace{
void print_test_name_about_to_run(std::ostream& o, const std::string& suite, const std::string& test){
	if(settings::inst().is_cout_terminal){
		o << "\e[1;33mrun\e[0m ";
	}else{
		o << "run ";
	}
	print_test_name(o, suite, test);
}
}

namespace{
void print_disabled_test_name(std::ostream& o, const std::string& suite, const std::string& test){
	if(settings::inst().is_cout_terminal){
		o << "\e[0;33mdisabled\e[0m ";
	}else{
		o << "disabled ";
	}
	print_test_name(o, suite, test);
}
}

namespace{
void print_failed_test_name(std::ostream& o, const std::string& suite, const std::string& test){
	if(settings::inst().is_cout_terminal){
		o << "\e[1;31mfailed\e[0m: ";
	}else{
		o << "failed: ";
	}
	print_test_name(o, suite, test);
}
}

namespace{
void print_error_info(std::ostream& o, const tst::check_failed& e){
	o << e.file << ":" << e.line;
	if(settings::inst().is_cout_terminal){
		o << ": \e[1;31merror\e[0m: ";
	}else{
		o << ": error: ";
	}
	o << e.message << std::endl;
}
}

namespace{
struct test_result{
	bool failed;
	std::string error_message;
};
}

namespace{
test_result run_test(const std::function<void()>& proc, const std::string& suite, const std::string& test_name){
	test_result res;

	res.failed = true;

	try{
		ASSERT(proc)
		proc();
		res.failed = false;
	}catch(tst::check_failed& e){
		std::stringstream ss;
		print_error_info(ss, e);
		res.error_message = ss.str();
	}catch(std::exception& e){
		std::stringstream ss;
		ss << "uncaught std::exception: " << e.what() << std::endl; // TODO: print exception type somehow???
		res.error_message = ss.str();
	}catch(...){
		res.error_message = "unknown exception caught\n";
	}

	return res;
}
}

namespace{
class runner : public nitki::thread{
	bool quit = false;
public:
	nitki::queue queue;

	void stop(){
		this->queue.push_back([this](){
			this->quit = true;
		});
	}

	void run()override{
		opros::wait_set wait_set(1);
		wait_set.add(this->queue, {opros::ready::read});

		while(true){
			wait_set.wait();
			if(this->queue.flags().get(opros::ready::read)){
				auto f = this->queue.pop_front();
				ASSERT(f)

				f();

				if(this->quit){
					return;
				}
			}
		}
	}
};
}

void tester::run(){
	// TODO: parallel run (check settings::num_threads)
	for(const auto& s : this->suites){
		for(const auto& p : s.second.procedures){
			if(!p.second){
				print_disabled_test_name(std::cout, s.first, p.first);
				++this->num_disabled;
				continue;
			}

			print_test_name_about_to_run(std::cout, s.first, p.first);

			auto res = run_test(p.second, s.first, p.first);

			if(res.failed){
				++this->num_failed;
				std::stringstream ss;
				print_failed_test_name(ss, s.first, p.first);
				ss << "  " << res.error_message;
				std::cout << ss.str();
			}else{
				++this->num_passed;
			}
		}
	}
}

size_t tester::size()const noexcept{
	size_t ret = 0;

	for(const auto& s : this->suites){
		ret += s.second.size();
	}

	return ret;
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

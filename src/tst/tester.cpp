#include "tester.hpp"

#include <iostream>
#include <sstream>
#include <vector>

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
// returns true if test has failed
bool run_test(const std::function<void()>& proc, const std::string& suite, const std::string& test_name){
	print_test_name_about_to_run(std::cout, suite, test_name);

	std::string error_message;

	try{
		ASSERT(proc)
		proc();
		return false;
	}catch(tst::check_failed& e){
		std::stringstream ss;
		print_error_info(ss, e);
		error_message = ss.str();
	}catch(std::exception& e){
		std::stringstream ss;
		ss << "uncaught std::exception: " << e.what() << std::endl; // TODO: print exception type somehow???
		error_message = ss.str();
	}catch(...){
		error_message = "unknown exception caught\n";
	}

	std::stringstream ss;
	print_failed_test_name(ss, suite, test_name);
	ss << "  " << error_message;
	std::cout << ss.str();

	return true;
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
		utki::scope_exit queue_scope_exit([&](){
			wait_set.remove(this->queue);
		});

		while(!this->quit){
			wait_set.wait();

			ASSERT(this->queue.flags().get(opros::ready::read))
			auto f = this->queue.pop_front();
			ASSERT(f)

			f();
		}
	}
};
}

namespace{
class runners_pool{
	std::vector<std::unique_ptr<runner>> runners;
	std::vector<runner*> free_runners;
public:
	void stop_all_runners(){
		for(auto& r : this->runners){
			r->stop();
		}
	}

	~runners_pool(){
		for(auto& r : this->runners){
			r->join();
		}
	}

	void free_runner(runner* r){
		ASSERT(std::find(this->free_runners.begin(), this->free_runners.end(), r) == this->free_runners.end(), [](auto&o){o << "runner is already freed";})
		this->free_runners.push_back(r);
	}

	runner* get_free_runner(){
		if(!this->free_runners.empty()){
			auto fr = this->free_runners.back();
			ASSERT(fr)
			this->free_runners.pop_back();
			return fr;
		}else if(this->runners.size() != settings::inst().num_threads){
			ASSERT(this->runners.size() < settings::inst().num_threads)
			this->runners.push_back(std::make_unique<runner>());
			auto fr = this->runners.back().get();
			fr->start();
			return fr;
		}
		return nullptr;
	}

	bool no_active_runners()const noexcept{
		return this->runners.size() == this->free_runners.size();
	}
};
}

int tester::run(){
	if(this->size() == 0){
		std::cout << "no tests to run" << std::endl;
		return 0;
	}

	class iterator{
		const decltype(tester::suites)& suites;

		decltype(tester::suites)::const_iterator si;
		decltype(suite::procedures)::const_iterator pi;
	public:
		iterator(decltype(suites)& suites) :
				suites(suites),
				si(suites.begin())
		{
			if(this->is_valid())
				this->pi = this->si->second.procedures.begin();
		}

		bool is_valid()const{
			return this->si != this->suites.end();
		}

		void next(){
			ASSERT(this->is_valid())
			++this->pi;
			if(this->pi == this->si->second.procedures.end()){
				++this->si;
				if(this->si != this->suites.end()){
					this->pi = this->si->second.procedures.begin();
				}
			}
		}

		const decltype(suite::procedures)::value_type::second_type& proc()const{
			ASSERT(this->is_valid())
			return this->pi->second;
		}
		const std::string& suite_name()const{
			ASSERT(this->is_valid())
			return this->si->first;
		}
		const std::string& test_name()const{
			ASSERT(this->is_valid())
			return this->pi->first;
		}
	};

	this->print_num_tests_about_to_run(std::cout);

	// set up queue for the main thread
	opros::wait_set wait_set(1);
	nitki::queue queue;
	wait_set.add(queue, {opros::ready::read});
	utki::scope_exit queue_scope_exit([&wait_set, &queue](){
		wait_set.remove(queue);
	});

	runners_pool pool;

	auto main_thread_id = std::this_thread::get_id();

	// TODO: add timeout
	iterator i(this->suites);
	while(true){
		if(i.is_valid()){
			if(!i.proc()){
				print_disabled_test_name(std::cout, i.suite_name(), i.test_name());
				++this->num_disabled;
				i.next();
				continue;
			}

			auto r = pool.get_free_runner();
			if(r){
				r->queue.push_back([
						&proc = i.proc(),
						&suite_name = i.suite_name(),
						&test_name = i.test_name(),
						&pool = pool,
						r,
						&queue = queue,
						this,
						&main_thread_id
					]()
				{
					bool failed = run_test(proc, suite_name, test_name);

					queue.push_back([failed, &pool = pool, r, this, &main_thread_id](){
						ASSERT(std::this_thread::get_id() == main_thread_id)
						pool.free_runner(r);
						if(failed){
							++this->num_failed;
						}else{
							++this->num_passed;
						}
					});
				});
				i.next();
				continue;
			}
		}else if(pool.no_active_runners()){
			// no tests left and no active runners
			break;
		}

		// no free runners, or no tests left, wait in the queue
		wait_set.wait();
		ASSERT(queue.flags().get(opros::ready::read))
		auto f = queue.pop_front();
		ASSERT(f)
		f();
	} // ~main loop

	this->print_num_tests_passed(std::cout);
	this->print_num_tests_disabled(std::cout);
	this->print_num_tests_failed(std::cout);
	this->print_outcome(std::cout);

	pool.stop_all_runners();

	return this->is_failed() ? 1 : 0;
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

void tester::print_outcome(std::ostream& o)const{
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

#include "tester.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include <utki/debug.hpp>
#include <nitki/queue.hpp>

#include "util.hxx"
#include "settings.hxx"
#include "iterator.hxx"
#include "runners_pool.hxx"
#include "reporter.hxx"

using namespace tst;

namespace{
void print_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\e[1;90m" << id.suite << "\e[0m \e[0;36m" << id.test << "\e[0m" << std::endl;
	}else{
		o << id.suite << " " << id.test << std::endl;
	}
}
}

namespace{
void print_test_name_about_to_run(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\e[1;33mrun\e[0m: ";
	}else{
		o << "run: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_disabled_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\e[0;33mdisabled\e[0m: ";
	}else{
		o << "disabled: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_failed_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\e[1;31mfailed\e[0m: ";
	}else{
		o << "failed: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_error_info(std::ostream& o, const tst::check_failed& e, bool color = settings::inst().is_cout_terminal){
	o << e.file << ":" << e.line;
	if(color){
		o << ": \e[1;31merror\e[0m: ";
	}else{
		o << ": error: ";
	}
	o << e.message;
}
}

namespace{
void run_test(const full_id& id, const std::function<void()>& proc, reporter& rep){
	print_test_name_about_to_run(std::cout, id);

	std::string console_error_message;

	try{
		ASSERT(proc)
		proc();
		rep.report_pass(id);
		return;
	}catch(tst::check_failed& e){
		{
			std::stringstream ss;
			print_error_info(ss, e);
			console_error_message = ss.str();
		}
		{
			std::stringstream ss;
			print_error_info(ss, e, false);
			rep.report_failure(id, ss.str());
		}
	}catch(std::exception& e){
		std::stringstream ss;
		ss << "uncaught std::exception: " << e.what(); // TODO: print exception type somehow???
		console_error_message = ss.str();
		rep.report_error(id, std::string(console_error_message));
	}catch(...){
		console_error_message = "uncaught unknown exception";
		rep.report_error(id, std::string(console_error_message));
	}

	std::stringstream ss;
	print_failed_test_name(ss, id);
	ss << "  " << console_error_message;
	std::cout << ss.str() << std::endl;
}
}

namespace{
auto main_thread_id = std::this_thread::get_id();
}

int tester::run(){
	if(this->size() == 0){
		std::cout << "no tests to run" << std::endl;
		return 0;
	}

	this->print_num_tests_about_to_run(std::cout);

	// set up queue for the main thread
	opros::wait_set wait_set(1);
	nitki::queue queue;
	wait_set.add(queue, {opros::ready::read});
	utki::scope_exit queue_scope_exit([&wait_set, &queue](){
		wait_set.remove(queue);
	});

	runners_pool pool;

	reporter rep(this->suites);

	// TODO: add timeout

	for(iterator i(this->suites); true;){
		if(i.is_valid()){
			auto& proc = i.info().proc;
			if(!proc){ // test has no precedure
				print_disabled_test_name(std::cout, i.id());
				rep.report_disabled_test();
				i.next();
				continue;
			}

			auto r = pool.occupy_runner();
			if(r){
				auto reply = [&pool, r](){
					ASSERT(std::this_thread::get_id() == main_thread_id)
					pool.free_runner(r);
				};

				r->queue.push_back([
						id = i.id(),
						&proc,
						&rep,
						&queue,
						reply = std::move(reply)
					]()
				{
					run_test(id, proc, rep);
					queue.push_back(std::move(reply));
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

	rep.print_num_tests_passed(std::cout);
	rep.print_num_tests_disabled(std::cout);
	rep.print_num_tests_failed(std::cout);
	rep.print_outcome(std::cout);

	pool.stop_all_runners();

	{
		auto& junit_file = settings::inst().junit_report_out_file;
		if(!junit_file.empty()){
			rep.write_junit_report(junit_file);
		}
	}

	return rep.is_failed() ? 1 : 0;
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

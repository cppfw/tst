#include "application.hpp"

#include <nitki/queue.hpp>

#include "util.hxx"
#include "settings.hxx"
#include "reporter.hxx"
#include "iterator.hxx"
#include "runners_pool.hxx"

using namespace tst;

application::application(
		std::string&& name,
		std::string&& description
	) :
		name(std::move(name)),
		description(std::move(description))
{
	this->cli.add("help", "display help information", [](){tst::settings::inst().show_help = true;});
	this->cli.add(
			'j',
			"jobs",
			"Number of parallel jobs. 0 = infinite jobs. Default value is 1.",
			[](std::string&& v){
				auto& s = tst::settings::inst();
				s.num_threads = std::stoul(v);
				if(s.num_threads == 0){
					s.num_threads = std::numeric_limits<decltype(s.num_threads)>::max();
				}
			}
		);
	this->cli.add(
			"junit-out",
			"Output filename of the test report in JUnit format.",
			[](std::string&& v){tst::settings::inst().junit_report_out_file = std::move(v);}
		);
	this->cli.add(
			"time-out-sec",
			"Time limit in seconds. Default value is 0, which means forever. After time out is hit, the program is aborted.",
			[](std::string&& v){tst::settings::inst().time_out = std::stoull(v);}
		);
	this->cli.add(
			'l',
			"list-tests",
			"List all tests without running them.",
			[](){tst::settings::inst().list_tests = true;}
		);
	this->cli.add(
			's',
			"suite",
			"Run tests from the given suite.",
			[](std::string&& v){
				// TODO:
			}
		);
	this->cli.add(
			't',
			"test",
			"Run only specified test from the suite indicated by --suite. The --suite value must be supplied.",
			[](std::string&& v){
				// TODO:
			}
		);
	this->cli.add(
			'r',
			"run-list",
			"Get list of tests to run from file in format:\n<suite1> <test1>\n  <test2>\n<suite2>\n<suite3>\n  <test3>\n...",
			[](std::string&& v){
				// TODO:
			}
		);
	this->cli.add(
			"run-list-stdin",
			"Get list of tests to run from stdin, in same format as for --run-list.",
			[](){
				// TODO:
			}
		);
	this->cli.add(
			"print-passed",
			"Print passed test name to stdout. By default, when test has passed, nothing is printed to stdout.",
			[](){settings::inst().print_passed = true;}
		);
}

void application::print_help()const{
	if(name.empty()){
		std::cout << "unit tests" << std::endl;
	}else{
		std::cout << name << std::endl;
	}
	if(!description.empty()){
		std::cout << description << std::endl;
	}
	std::cout << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << cli.description();
}

size_t application::num_tests()const noexcept{
	size_t ret = 0;

	for(const auto& s : this->suites){
		ret += s.second.size();
	}

	return ret;
}

suite& application::create_suite(const std::string& id){
	validate_id(id);

	auto i = this->suites.emplace(id, suite());
	if(!i.second){
		std::stringstream ss;
		ss << "could not create test suite: suite with id '" << id << "' already exists";
		throw std::invalid_argument(ss.str());
	}
	return i.first->second;
}

void application::list_tests(std::ostream& o)const{
	for(const auto& s : this->suites){
		o << s.first << '\n';
		for(const auto& t : s.second.tests){
			o << '\t' << t.first << '\n';
		}
	}
}

namespace{
void print_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\033[1;90m" << id.suite << "\033[0m \033[0;36m" << id.test << "\033[0m";
	}else{
		o << id.suite << " " << id.test;
	}
	o << '\n';
}
}

namespace{
void print_test_name_about_to_run(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\033[1;33mrun\033[0m: ";
	}else{
		o << "run: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_disabled_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\033[0;33mdisabled\033[0m: ";
	}else{
		o << "disabled: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_failed_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().is_cout_terminal){
		o << "\033[1;31mfailed\033[0m: ";
	}else{
		o << "failed: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_passed_test_name(std::ostream& o, const full_id& id){
	if(!settings::inst().print_passed){
		return;
	}
	if(settings::inst().is_cout_terminal){
		o << "\033[1;32mpassed\033[0m: ";
	}else{
		o << "passed: ";
	}
	print_test_name(o, id);
}
}

namespace{
void print_error_info(std::ostream& o, const tst::check_failed& e, bool color = settings::inst().is_cout_terminal){
	o << e.file << ":" << e.line;
	if(color){
		o << ": \033[1;31merror\033[0m: ";
	}else{
		o << ": error: ";
	}
	o << e.message;
}
}

namespace{
void run_test(const full_id& id, const std::function<void()>& proc, reporter& rep){
	{
		std::stringstream ss;
		print_test_name_about_to_run(ss, id);
		std::cout << ss.str();
	}

	std::string console_error_message;

	try{
		ASSERT(proc)
		proc();
		rep.report_pass(id);

		std::stringstream ss;
		print_passed_test_name(ss, id);

		std::cout << ss.str();
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

	{
		std::stringstream ss;
		print_failed_test_name(ss, id);
		ss << "  " << console_error_message << '\n';
		std::cout << ss.str();
	}
}
}

namespace{
auto main_thread_id = std::this_thread::get_id();
}

int application::run(){
	if(this->num_tests() == 0){
		std::cout << "no tests to run" << std::endl;
		return 0;
	}

	// set up queue for the main thread
	opros::wait_set wait_set(1);
	nitki::queue queue;
	wait_set.add(queue, {opros::ready::read});
	utki::scope_exit queue_scope_exit([&wait_set, &queue](){
		wait_set.remove(queue);
	});

	runners_pool pool;

	reporter rep(*this);

	rep.print_num_tests_about_to_run(std::cout);

	// TODO: add timeout

	for(iterator i(this->suites); true;){
		if(i.is_valid()){
			auto& proc = i.info().proc;
			if(!proc){ // test has no precedure
				print_disabled_test_name(std::cout, i.id());
				rep.report_disabled_test(i.id());
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

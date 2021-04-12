#include "application.hpp"

#include <utki/time.hpp>

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

	ASSERT(proc)
	uint32_t start_ticks = utki::get_ticks_ms();
	try{
		proc();
		uint32_t dt = utki::get_ticks_ms() - start_ticks;
		rep.report_pass(id, dt);

		std::stringstream ss;
		print_passed_test_name(ss, id);

		std::cout << ss.str();
		return;
	}catch(tst::check_failed& e){
		uint32_t dt = utki::get_ticks_ms() - start_ticks;
		{
			std::stringstream ss;
			print_error_info(ss, e);
			console_error_message = ss.str();
		}
		{
			std::stringstream ss;
			print_error_info(ss, e, false);
			rep.report_failure(id, dt, ss.str());
		}
	}catch(std::exception& e){
		uint32_t dt = utki::get_ticks_ms() - start_ticks;
		std::stringstream ss;
		ss << "uncaught std::exception: " << e.what(); // TODO: print exception type somehow???
		console_error_message = ss.str();
		rep.report_error(id, dt, std::string(console_error_message));
	}catch(...){
		uint32_t dt = utki::get_ticks_ms() - start_ticks;
		console_error_message = "uncaught unknown exception";
		rep.report_error(id, dt, std::string(console_error_message));
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

	uint32_t start_ticks = utki::get_ticks_ms();

	for(iterator i(this->suites); true;){
		if(i.is_valid()){
			auto id = i.id();
			if(!this->is_in_run_list(id.suite, id.test)){
				rep.report_skipped(i.id(), "not in run list");
				i.next();
				continue;
			}

			auto& proc = i.info().proc;
			if(!proc){ // test has no precedure
				print_disabled_test_name(std::cout, id);
				rep.report_disabled_test(id);
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
						id,
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

	rep.time_ms = utki::get_ticks_ms() - start_ticks;

	rep.print_num_tests_run(std::cout);
	rep.print_num_tests_passed(std::cout);
	rep.print_num_tests_disabled(std::cout);
	rep.print_num_tests_skipped(std::cout);
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

bool application::is_in_run_list(const std::string& suite, const std::string& test)const{
	if(this->run_list.empty()){
		return true;
	}
	auto si = this->run_list.find(suite);
	if(si == this->run_list.end()){
		return false;
	}
	const auto& set = si->second;
	if(!set.empty()){
		auto ti = set.find(test);
		if(ti == set.end()){
			return false;
		}
	}
	return true;
}

namespace{
void skip_indentation(std::istream& is){
	if(is.eof()){
		return;
	}
	auto c = is.peek();
	while(!is.eof() && (c == ' ' || c == '\t')){
		is.get();
		c = is.peek();
	}
}
}

namespace{
void skip_to_new_line(std::istream& is){
	if(is.eof()){
		return;
	}
	auto c = is.get();
	while(!is.eof() && c != '\n'){
		c = is.get();
	}
}
}

namespace{
std::string read_in_name(std::istream& is){
	if(is.eof()){
		return std::string();
	}
	std::stringstream ss;
	auto c = is.peek();
	while(!is.eof() && is_valid_id_char(c)){
		ss << char(is.get());
		c = is.peek();
	}
	return ss.str();
}
}

namespace{
void throw_syntax_error_invalid_char(size_t line, char c){
	std::stringstream ss;
	ss << "error in run list syntax at line: " << line << ": invalid character 0x" << std::hex << unsigned(c);
	throw std::invalid_argument(ss.str());
}
}

void application::read_run_list_from_stdin(){
	if(utki::is_cin_terminal()){
		return;
	}

	bool expect_test_name = false;

	std::string_view cur_suite_name;
	const suite* cur_suite = nullptr;
	decltype(this->run_list)::value_type::second_type* cur_run_list_suite = nullptr;

	std::istream& is = std::cin;

	size_t line = 0;

	for(char c = is.peek(); !is.eof(); c = is.peek()){
		// LOG([&](auto&o){o << "line = " << line << " c = " << c << '\n';})
		ASSERT(!is.eof())
		ASSERT(!is.fail())

		if(expect_test_name){
			switch(c){
				case '\r':
				case '\n':
				case '#':
					break;
				default:
					skip_indentation(is);
					if(is_valid_id_char(c)){
						auto tn = read_in_name(is);
						// LOG([&](auto&o){o << "test parsed: " << tn << '\n';})
						ASSERT(cur_suite)

						auto i = cur_suite->tests.find(tn);
						if(i == cur_suite->tests.end()){
							std::stringstream ss;
							ss << "test '" << tn << "' not found in suite '" << cur_suite_name << '\'';
						}
						ASSERT(cur_run_list_suite)
						cur_run_list_suite->insert(std::string_view(i->first));
					}else{
						throw_syntax_error_invalid_char(line, c);
					}
					break;
			}
			skip_to_new_line(is);
			++line;
			expect_test_name = false;
		}else{
			switch(c){
				case '#':
					skip_to_new_line(is);
					++line;
					break;
				default:
					if(is_valid_id_char(c)){
						auto sn = read_in_name(is);
						// LOG([&](auto&o){o << "suite parsed: " << sn << '\n';})

						auto i = this->suites.find(sn);
						if(i == this->suites.end()){
							std::stringstream ss;
							ss << "suite '" << sn << "' not found";
							throw std::invalid_argument(ss.str());
						}
						cur_suite_name = std::string_view(i->first);
						cur_suite = &i->second;
						cur_run_list_suite = &this->run_list[cur_suite_name];
					}else{
						throw_syntax_error_invalid_char(line, c);
					}
					// fall-through
				case ' ':
				case '\t':
					skip_indentation(is);
					expect_test_name = true;
					break;
			}
		}
	}
}

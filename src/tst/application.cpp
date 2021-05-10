#include "application.hpp"

#include <utki/config.hpp>
#include <utki/time.hpp>

#include <nitki/queue.hpp>

#if M_COMPILER == M_COMPILER_GCC || M_COMPILER == M_COMPILER_CLANG
#	include <cxxabi.h>
#endif

#include "set.hpp"

#include "util.hxx"
#include "settings.hxx"
#include "reporter.hxx"
#include "iterator.hxx"
#include "runners_pool.hxx"

using namespace tst;

application_factory::factory_type& application_factory::get_factory(){
	static application_factory::factory_type f;
	return f;
}

application_factory::application_factory(factory_type&& factory){
	auto& f = this->get_factory();
	if(f){
		throw std::logic_error("application factory is already registered");
	}
	f = std::move(factory);
}

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
			"Number of parallel jobs. Possible values:" "\n"
			"  positive non-zero number = number of concurrent jobs." "\n"
			"  max = unlimited number of concurrent jobs." "\n"
			"  auto = number of physical threads supported by the system." "\n"
			"Default value is 1.",
			[](std::string&& v){
				auto& s = tst::settings::inst();
				if(v == "auto"){
					s.num_threads = std::thread::hardware_concurrency();
				}else if(v == "max"){
					s.num_threads = std::numeric_limits<decltype(s.num_threads)>::max();
				}else{
					s.num_threads = std::stoul(v);
					if(s.num_threads == 0){
						throw std::invalid_argument("--jobs argument value must not be 0");
					}
				}
			}
		);
	this->cli.add(
			"junit-out",
			"Output filename of the test report in JUnit format.",
			[](std::string&& v){tst::settings::inst().junit_report_out_file = std::move(v);}
		);
	this->cli.add(
			'l',
			"list-tests",
			"List all tests without running them.",
			[](){tst::settings::inst().list_tests = true;}
		);
	this->cli.add(
			"about-to-run",
			"Print name of the test about to run. By default, before the test is sun its name is not printed.",
			[](){settings::inst().print_about_to_run = true;}
		);
	this->cli.add(
			"passed",
			"Print passed tests name to stdout. By default, when test has passed, nothing is printed to stdout.",
			[](){settings::inst().print_passed = true;}
		);
	this->cli.add(
			"skipped",
			"Print skipped tests name to stdout. By default, when test has been skipped, nothing is printed to stdout.",
			[](){settings::inst().print_skipped = true;}
		);
	this->cli.add(
			"outcome",
			"Print overall testing outcome. By default, overall testing outcome is not printed.",
			[](){settings::inst().print_outcome = true;}
		);
	this->cli.add(
			"no-color",
			"Do not use output coloring even if running from terminal.",
			[](){settings::inst().colored_output = false;}
		);
	this->cli.add(
			"run-disabled",
			"Run disabled tests as well.",
			[](){settings::inst().run_disabled = true;}
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

size_t application::run_list_size()const noexcept{
	size_t ret = 0;

	for(const auto& s : this->run_list){
		ret += s.second.size();
	}

	return ret;
}

suite& application::get_suite(const std::string& id){
	validate_id(id);

	auto i = this->suites.emplace(id, suite());
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
	if(settings::inst().colored_output){
		o << "\033[2;36m" << id.suite << "\033[0m \033[0;36m" << id.test << "\033[0m";
	}else{
		o << id.suite << " " << id.test;
	}
	o << '\n';
}
}

namespace{
void print_test_name_about_to_run(std::ostream& o, const full_id& id){
	if(!settings::inst().print_about_to_run){
		return;
	}
	std::stringstream ss;
	if(settings::inst().colored_output){
		ss << "\033[1;33mrun\033[0m: ";
	}else{
		ss << "run: ";
	}
	print_test_name(ss, id);
	o << ss.str();
}
}

namespace{
void print_disabled_test_name(std::ostream& o, const full_id& id){
	std::stringstream ss;
	if(settings::inst().colored_output){
		ss << "\033[0;33mdisabled\033[0m: ";
	}else{
		ss << "disabled: ";
	}
	print_test_name(ss, id);
	o << ss.str();
}
}

namespace{
void print_skipped_test_name(std::ostream& o, const full_id& id){
	if(!settings::inst().print_skipped){
		return;
	}
	std::stringstream ss;
	if(settings::inst().colored_output){
		ss << "\033[1;90mskipped\033[0m: ";
	}else{
		ss << "skipped: ";
	}
	print_test_name(ss, id);
	o << ss.str();
}
}

namespace{
void print_failed_test_name(std::ostream& o, const full_id& id){
	if(settings::inst().colored_output){
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
	std::stringstream ss;
	if(settings::inst().colored_output){
		ss << "\033[1;32mpassed\033[0m: ";
	}else{
		ss << "passed: ";
	}
	print_test_name(ss, id);
	o << ss.str();
}
}

namespace{
void print_error_info(std::ostream& o, const tst::check_failed& e, bool color = settings::inst().colored_output){
	o << e.source_location.file_name() << ":" << e.source_location.line();
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
	print_test_name_about_to_run(std::cout, id);

	std::string console_error_message;

	ASSERT(proc)
	uint32_t start_ticks = utki::get_ticks_ms();
	try{
		proc();
		uint32_t dt = utki::get_ticks_ms() - start_ticks;
		rep.report_pass(id, dt);

		print_passed_test_name(std::cout, id);
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
		ss << "uncaught " << 
#if M_COMPILER == M_COMPILER_GCC || M_COMPILER == M_COMPILER_CLANG
				abi::__cxa_demangle(typeid(e).name(), 0, 0, nullptr)
#else
				typeid(e).name()
#endif
				<< ": " << e.what();
		console_error_message = ss.str();
		rep.report_error(id, dt, std::string(console_error_message));
	}catch(...){
		uint32_t dt = utki::get_ticks_ms() - start_ticks;
		std::stringstream ss;
		ss << "uncaught " <<
#if M_COMPILER == M_COMPILER_GCC || M_COMPILER == M_COMPILER_CLANG
				abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, nullptr)
#else
				"unknown exception"
#endif
			;
		console_error_message = ss.str();
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
				print_skipped_test_name(std::cout, id);
				rep.report_skipped(id, "not in run list");
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
							throw std::invalid_argument(ss.str());
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
				case '\n':
				case '\r':
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

void application::init(){
	for(const auto& i : set::get_inits()){
		auto& s = this->get_suite(i.first);
		for(const auto& p : i.second){
			ASSERT(p)
			p(s);
		}
	}

	set::get_inits().clear(); // we don't need that anymore
}

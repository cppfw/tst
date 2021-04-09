#include <clargs/parser.hpp>

#include <utki/util.hpp>

#include "settings.hxx"

#include "tester.hpp"
#include "init.hpp"

namespace{
void add_command_line_arguments(clargs::parser& p){
	p.add("help", "display help information", [](){tst::settings::inst().show_help = true;});
	p.add(
			'j',
			"jobs",
			"Number of parallel jobs. 0 = infinite jobs. Default value is 1.",
			[](std::string&& v){tst::settings::inst().num_threads = std::stoul(v);}
		);
	p.add(
			"junit-out",
			"Output filename of the test report in JUnit format.",
			[](std::string&& v){tst::settings::inst().junit_report_out_file = std::move(v);}
		);
	p.add(
			't',
			"time-out-sec",
			"Time limit in seconds. Default value is 0, which means forever. After time out is hit, the program is aborted.",
			[](std::string&& v){tst::settings::inst().time_out = std::stoull(v);}
		);
}
}

namespace{
void print_help(const tst::tester& t){
	if(t.description.empty()){
		std::cout << "unit tests" << std::endl;
	}else{
		std::cout << t.description << std::endl;
	}
	std::cout << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << t.cli.description();
}
}

namespace tst{

int main(utki::span<const char*> args){
	settings settings_singleton;

	tst::tester t;

	add_command_line_arguments(t.cli);

	if(!tst::init(t)){
		return 0;
	}

	t.cli.parse(args);

	if(settings::inst().show_help){
		print_help(t);
		return 0;
	}

	// print number of tests about to run
	if(settings::inst().is_cout_terminal){
		std::cout << "\e[1;33;4mrunning\e[0m ";
	}else{
		std::cout << "running ";
	}
	std::cout << t.size() << " test(s)" << std::endl;

	t.run();

	// print number of passed tests
	if(settings::inst().is_cout_terminal){
		std::cout << "\e[1;32m" << t.num_passed << "\e[0m";
	}else{
		std::cout << t.num_passed;
	} 
	std::cout << " test(s) passed" << std::endl;

	// print number of disabled tests
	if(t.num_disabled != 0){
		if(settings::inst().is_cout_terminal){
			std::cout << "\e[0;33m" << t.num_disabled << "\e[0m";
		}else{
			std::cout << t.num_disabled;
		}
		std::cout << " test(s) disabled" << std::endl;
	}

	if(t.num_failed != 0){
		// print number of failed tests
		if(settings::inst().is_cout_terminal){
			std::cout << "\e[1;31m" << t.num_failed  << "\e[0m";
		}else{
			std::cout << t.num_failed;
		}
		std::cout << " test(s) failed" << std::endl;

		// print FAILED word
		if(settings::inst().is_cout_terminal){
			std::cout << "\t\e[1;31mFAILED\e[0m" << std::endl;
		}else{
			std::cout << "\tFAILED" << std::endl;
		}
		return 1;
	}else{
		// print PASSED word
		if(settings::inst().is_cout_terminal){
			std::cout << "\t\e[1;32mPASSED\e[0m" << std::endl;
		}else{
			std::cout << "\tPASSED" << std::endl;
		}
		return 0;
	}
}

}

int main(int argc, const char** argv){
	return tst::main(utki::make_span(argv, argc));
}

#include <clargs/parser.hpp>

#include <utki/util.hpp>

#include "settings.hxx"

#include "tester.hpp"
#include "init.hpp"

namespace{
void define_command_line_arguments(clargs::parser& p){
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

namespace{
void print_outcome(bool failed){
	if(failed){
		// print FAILED word
		if(tst::settings::inst().is_cout_terminal){
			std::cout << "\t\e[1;31mFAILED\e[0m" << std::endl;
		}else{
			std::cout << "\tFAILED" << std::endl;
		}
	}else{
		// print PASSED word
		if(tst::settings::inst().is_cout_terminal){
			std::cout << "\t\e[1;32mPASSED\e[0m" << std::endl;
		}else{
			std::cout << "\tPASSED" << std::endl;
		}
	}
}
}

namespace tst{

int main(utki::span<const char*> args){
	settings settings_singleton;

	tst::tester t;

	define_command_line_arguments(t.cli);

	if(!tst::init(t)){
		return 0;
	}

	t.cli.parse(args);

	if(settings::inst().show_help){
		print_help(t);
		return 0;
	}

	t.print_num_tests_about_to_run(std::cout);

	t.run();

	t.print_num_tests_passed(std::cout);
	t.print_num_tests_disabled(std::cout);
	t.print_num_tests_failed(std::cout);

	print_outcome(t.is_failed());

	return t.is_failed() ? 1 : 0;
}

}

int main(int argc, const char** argv){
	return tst::main(utki::make_span(argv, argc));
}

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
			[](std::string&& v){
				auto& s = tst::settings::inst();
				s.num_threads = std::stoul(v);
				if(s.num_threads == 0){
					s.num_threads = std::numeric_limits<decltype(s.num_threads)>::max();
				}
			}
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
	p.add(
			'l',
			"run-list-file",
			"get list of tests to run from file in format:\n<suite1> <test1>\n<suite1> <test2>\n<suite2>\n<suite3> <test3>\n...",
			[](std::string&& v){
				// TODO:
			}
		);
}
}

namespace{
void print_help(const tst::tester& t){
	if(t.name.empty()){
		std::cout << "unit tests" << std::endl;
	}else{
		std::cout << t.name << std::endl;
	}
	if(!t.description.empty()){
		std::cout << t.description << std::endl;
	}
	std::cout << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << t.cli.description();
}
}

namespace{
decltype(tst::init)* load_init_function(){
	return &tst::init;
}
}

namespace tst{

int main(utki::span<const char*> args){
	settings settings_singleton;

	tst::tester t;

	define_command_line_arguments(t.cli);

	{
		auto f = load_init_function();
		ASSERT(f)
		if(!f(t)){
			return 0;
		}
	}

	t.cli.parse(args);

	if(settings::inst().show_help){
		print_help(t);
		return 0;
	}

	return t.run();
}

}

int main(int argc, const char** argv){
	return tst::main(utki::make_span(argv, argc));
}

#pragma once

#include <utki/singleton.hpp>
#include <utki/util.hpp>

namespace tst{

class settings : public utki::intrusive_singleton<settings>{
	friend T_Singleton;
	static T_Instance instance;
public:

	bool colored_output = utki::is_cout_terminal();

	bool show_help = false;

	bool list_tests = false;

	bool print_about_to_run = false;
	bool print_passed = false;
	bool print_skipped = false;
	bool print_outcome = false;

	unsigned long num_threads = 1;

	std::string junit_report_out_file;
};

}

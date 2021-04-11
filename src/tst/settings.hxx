#pragma once

#include <utki/singleton.hpp>
#include <utki/util.hpp>

namespace tst{

class settings : public utki::intrusive_singleton<settings>{
	friend T_Singleton;
	static T_Instance instance;
public:

	bool is_cout_terminal = utki::is_cout_terminal();

	bool show_help = false;

	bool list_tests = false;

	unsigned long num_threads = 1;

	std::string junit_report_out_file;

	uint64_t time_out = 0;
};

}

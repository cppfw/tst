#pragma once

#include <utki/singleton.hpp>
#include <utki/util.hpp>

namespace tst{

struct settings : public utki::singleton<settings>{
	bool is_cout_terminal = utki::is_cout_terminal();

	bool show_help = false;

	unsigned long num_threads = 1;

	std::string junit_report_out_file;

	uint64_t time_out = 0;
};

}

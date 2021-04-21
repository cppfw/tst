#include <clargs/parser.hpp>

#include <utki/util.hpp>
#include <utki/config.hpp>

#if M_OS == M_OS_WINDOWS
#	include <utki/windows.hpp>
#else
#	include <dlfcn.h>
#endif

#include "settings.hxx"
#include "util.hxx"

#include "application.hpp"

namespace tst{
int main(utki::span<const char*> args){
	settings settings_singleton;

	std::unique_ptr<application> app;

	auto& factory = application_factory::get_factory();
	if(factory){
		app = factory();

		if(!app){
			throw std::logic_error("tst::create_application() returned nullptr");
		}
	}else{
		LOG([](auto&o){o << "tst::create_application() function not found, creating basic application" << '\n';})
		app = std::make_unique<application>("tests", std::string());
	}

	app->cli.parse(args);

	if(settings::inst().show_help){
		app->print_help();
		return 0;
	}

	app->init();

	if(settings::inst().list_tests){
		app->list_tests(std::cout);
		return 0;
	}

	app->read_run_list_from_stdin();

	return app->run();
}
}

int main(int argc, const char** argv){
	return tst::main(utki::make_span(argv, argc));
}

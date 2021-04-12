#include <clargs/parser.hpp>

#include <utki/util.hpp>

#include "settings.hxx"
#include "util.hxx"

#include "application.hpp"

namespace{
void print_help(const std::string& name, const std::string& description, const clargs::parser& cli){
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
}

namespace tst{

int main(utki::span<const char*> args){
	settings settings_singleton;

	std::unique_ptr<application> app;
	// initialize tests
	{
		auto factory = load_create_application_function();
		ASSERT(factory)
		app = factory();
	}

	if(!app){
		throw std::logic_error("tst::create_application() returned nullptr");
	}

	app->cli.parse(args);

	if(settings::inst().show_help){
		print_help(app->name, app->description, app->cli);
		return 0;
	}

	bool run_tests = app->init();

	if(settings::inst().list_tests){
		app->list_tests(std::cout);
		return 0;
	}

	if(!run_tests){
		return 0;
	}

	return app->run();
}

}

int main(int argc, const char** argv){
	return tst::main(utki::make_span(argv, argc));
}

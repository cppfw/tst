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

namespace{
decltype(tst::create_application)* load_create_application_function(){
	decltype(tst::create_application)* factory;
	static const auto gcc_mangling = "_ZN3tst18create_applicationEv";
#if M_OS == M_OS_WINDOWS
	// try gcc function name mangling first
	factory = reinterpret_cast<decltype(factory)>(
			GetProcAddress(
					GetModuleHandle(NULL),
					TEXT(gcc_mangling)
				)
		);
	
	if(!factory){ // try MSVC function name mangling style
		factory = reinterpret_cast<decltype(factory)>(
				GetProcAddress(
						GetModuleHandle(NULL),
						TEXT("?create_application@tst@@YA?AV?$unique_ptr@Vapplication@tst@@U?$default_delete@Vapplication@tst@@@std@@@std@@XZ")
					)
			);
	}
#else
	void* lib_handle = dlopen(nullptr, RTLD_NOW);
	
	if(!lib_handle){
		throw std::runtime_error("dlopen(): failed");
	}

	utki::scope_exit lib_handle_scope_exit([lib_handle](){
		dlclose(lib_handle);
	});

	factory = reinterpret_cast<decltype(factory)>(
			dlsym(lib_handle, gcc_mangling)
		);
#endif
	return factory;
}
}

namespace tst{
int main(utki::span<const char*> args){
	settings settings_singleton;

	auto factory = load_create_application_function();
	if(!factory){
		throw std::runtime_error("load_create_application_function(): create_application() function not found!");
	}
	auto app = factory();

	if(!app){
		throw std::logic_error("tst::create_application() returned nullptr");
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

	return app->run();
}
}

int main(int argc, const char** argv){
	return tst::main(utki::make_span(argv, argc));
}

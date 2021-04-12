#include "util.hxx"

#include <algorithm>
#include <sstream>

#include <utki/config.hpp>

#if M_OS == M_OS_WINDOWS
#	include <utki/windows.hpp>
#else
#	include <dlfcn.h>
#endif

void tst::validate_id(const std::string& id){
	auto i = std::find_if(
			id.begin(),
			id.end(),
			[](std::remove_reference_t<decltype(id)>::value_type c){
				return !(
						('a' <= c && c <= 'z') ||
						('A' <= c && c <= 'Z') ||
						('0' <= c && c <= '9') ||
						c == '_' ||
						c == '[' ||
						c == ']'
					);
			}
		);
	if(i != id.end()){
		std::stringstream ss;
		ss << "test case id validation failed: character '" << *i << "' is not allowed; id = " << id;
		throw std::invalid_argument(ss.str());
	}
}

decltype(tst::create_application)* tst::load_create_application_function(){
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
	if(!factory){
		throw std::runtime_error("load_create_application_function(): create_application() function not found!");
	}
	return factory;
}

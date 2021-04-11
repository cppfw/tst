#include "util.hxx"

#include <algorithm>
#include <sstream>

#include <utki/config.hpp>

#if M_OS == M_OS_WINDOWS
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
						c == ']' ||
						c == '(' ||
						c == ')'
					);
			}
		);
	if(i != id.end()){
		std::stringstream ss;
		ss << "test case id validation failed: character '" << *i << "' is not allowed; id = " << id;
		throw std::invalid_argument(ss.str());
	}
}

decltype(tst::init)* tst::load_init_function(){
	decltype(tst::init)* factory;
#if M_OS == M_OS_WINDOWS
	// Try GCC name mangling first
	factory = reinterpret_cast<decltype(factory)>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN3tst4initERNS_6testerE")));

	if(!factory){ // try MSVC function mangling style
		factory = reinterpret_cast<decltype(factory)>(GetProcAddress(
				GetModuleHandle(NULL),
#	if M_CPU == M_CPU_X86_64
				TEXT("?create_application@mordavokne@@YA?AV?$unique_ptr@Vapplication@mordavokne@@U?$default_delete@Vapplication@mordavokne@@@std@@@std@@HPEAPEBD@Z")
#	else
				TEXT("?create_application@mordavokne@@YA?AV?$unique_ptr@Vapplication@mordavokne@@U?$default_delete@Vapplication@mordavokne@@@std@@@std@@HPAPBD@Z")
#	endif
			));
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
			dlsym(lib_handle, "_ZN3tst4initERNS_6testerE")
		);
#endif
	if(!factory){
		throw std::runtime_error("load_init_function(): tst::init(tester&) function not found!");
	}
	return factory;
}

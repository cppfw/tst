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
	void* lib_handle = dlopen(nullptr, RTLD_NOW);
	if(!lib_handle){
		throw std::runtime_error("dlopen(): failed");
	}

	utki::scope_exit lib_handle_scope_exit([lib_handle](){
		dlclose(lib_handle);
	});

	auto mangled_name = "_ZN3tst4initERNS_6testerE";

	auto factory = reinterpret_cast<decltype(tst::init)*>(
			dlsym(lib_handle, mangled_name)
								//   _ZN3tst4initERNS_6testerE
			// dlsym(lib_handle, "_GLOBAL__sub_I__ZN3tst4initERNS_6testerE")
		);

	if(!factory){
		std::stringstream ss;
		ss << "dlsym(): tst::init(tester&) function not found! mangled_name = " << mangled_name;
		throw std::runtime_error(ss.str());
	}

	return factory;
}

#include "check.hpp"

#include "util.hxx"

using namespace tst;

void tst::check(
		bool c,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	)
{
	if(c){
		return;
	}

	std::stringstream ss;

	if(print){
		print(ss);
	}else{
		ss << "condition was false";
	}

	throw check_failed(ss.str(), source_location);
}

#include "check.hpp"

#include "util.hxx"

using namespace tst;

void tst::check(
		bool c,
		const std::function<void(std::ostream&)>& print,
		const std::pair<const char*, size_t>& source_location
	)
{
	if(c){
		return;
	}

	std::stringstream ss;

	if(print){
		print(ss);
	}

	throw check_failed(ss.str(), source_location);
}

void tst::check(bool c, const std::pair<const char*, size_t>& source_location){
	tst::check(
			c,
			[](auto& o){
				o << "condition was false";
			},
			source_location
		);
}

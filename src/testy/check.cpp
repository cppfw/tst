#include "check.hpp"

#include "check_failed.hxx"

using namespace testy;

void testy::check(bool c, const std::pair<const char*, size_t>& source_location){
	check(c, [](){return "condition was false";}, source_location);
}

void testy::check(
		bool c,
		const std::function<std::string()>& message_factory,
		const std::pair<const char*, size_t>& source_location
	)
{
	if(c){
		return;
	}

	throw check_failed(
			message_factory ? message_factory() : nullptr,
			source_location
		);
}

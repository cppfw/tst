#include "check.hpp"

#include "util.hxx"

using namespace tst;

namespace{
const char* default_fail_message = "check(false)";
}

void tst::check(
		bool c,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
	)
{
	if(c){
		return;
	}

	std::stringstream ss;

	if(print){
		print(ss);
	}else{
		ss << default_fail_message;
	}

	throw check_failed(ss.str(), std::move(source_location));
}

check_result::~check_result()noexcept(false){
	if(!this->failed){
		return;
	}
	
	std::string message;
	try{
		message = this->ss.str();
		if(message.empty()){
			message = default_fail_message;
		}
	}catch(...){}
	throw check_failed(std::move(message), std::move(this->source_location));
}

check_result tst::check(
		bool c,
		utki::source_location&& source_location
	)
{
	if(c){
		return check_result();
	}

	return check_result(std::move(source_location));
}

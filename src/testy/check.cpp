#include "check.hpp"

#include "check_failed.hxx"

using namespace testy;

void testy::check(bool c, file_line fl){
	if(!c){
		throw check_failed();
	}
}

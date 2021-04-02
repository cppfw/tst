#include "assert.hpp"

#include "assertion_failed.hxx"

using namespace testu;

void testu::assert_true(bool c){
	if(!c){
		throw assertion_failed();
	}
}

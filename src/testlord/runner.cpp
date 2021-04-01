#include "runner.hpp"

#include <sstream>

using namespace testlord;

void runner::add(std::string&& name, std::function<void()>&& proc){
	auto i = this->procedures.find(name);
	if(i != this->procedures.end()){
		std::stringstream ss;
		ss << "test with name '" << name << "' is already added";
		throw std::invalid_argument(ss.str());
	}
}

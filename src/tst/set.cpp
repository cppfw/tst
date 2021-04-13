#include "set.hpp"

using namespace tst;

set::inits_type& set::get_inits(){
	static inits_type inits;
	return inits;
}

set::set(const std::string& suite_name, inits_type::value_type::second_type::value_type&& init){
	get_inits()[suite_name].push_back(std::move(init));
}

#include "set.hpp"

using namespace tst;

decltype(set::inits) set::inits;

set::set(const std::string& suite_name, decltype(inits)::value_type::second_type::value_type&& init){
	inits[suite_name].push_back(std::move(init));
}

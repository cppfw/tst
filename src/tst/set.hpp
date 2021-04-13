#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

#include "suite.hpp"
namespace tst{

class set{
	friend class application;
	
	static std::unordered_map<std::string, std::vector<std::function<void(suite&)>>> inits;
public:
	set(const std::string& suite_name, decltype(inits)::value_type::second_type::value_type&& init);
};

}

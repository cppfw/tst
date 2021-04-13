#pragma once

#include <map>
#include <vector>
#include <functional>

#include "suite.hpp"
namespace tst{

class set{
	friend class application;
	
	typedef std::map<std::string, std::vector<std::function<void(suite&)>>> inits_type;

	// The static inits map object has to be returned by getter function to avoid
	// undetermined initialization order of global variables problem.
	static inits_type& get_inits();
public:
	set(const std::string& suite_name, inits_type::value_type::second_type::value_type&& init);
};

}

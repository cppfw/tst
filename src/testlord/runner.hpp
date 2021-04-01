#pragma once

#include <functional>

namespace testlord{

class runner{
	runner(){}
public:
	void add(std::string&& name, std::function<void()>&& proc);

};

}

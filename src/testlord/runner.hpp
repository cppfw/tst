#pragma once

#include <functional>
#include <map>

namespace testlord{

class runner{
	runner(){}

	std::map<std::string, std::function<void()>> procedures;

	void run();
public:
	void add(std::string&& name, std::function<void()>&& proc);

	template <class fixture>
	void add(std::string&& name, std::function<void(fixture&)>&& proc){
		this->add(
				std::move(name),
				[proc = std::move(proc)](){
					fixture f;
					proc(f);
				}
			);
	}
};

}

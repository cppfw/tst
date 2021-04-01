#pragma once

#include <functional>
#include <map>

namespace testlord{

class runner{
	runner(){}

	std::map<std::string, std::function<void()>> procedures;

	void run();
public:
	void add(const std::string& id, std::function<void()>&& proc);

	template <class fixture>
	void add(const std::string& id, std::function<void(fixture&)>&& proc){
		this->add(
				id,
				[proc = std::move(proc)](){
					fixture f;
					proc(f);
				}
			);
	}
};

}

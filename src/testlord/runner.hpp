#pragma once

#include <functional>
#include <map>
#include <vector>

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

	template <class Param>
	void add(const std::string& id, std::vector<Param>&& params, std::function<void(const Param&)>&& proc){
		// TODO:
	}
};

}

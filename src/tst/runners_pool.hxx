#pragma once

#include <vector>

#include "runner.hxx"

namespace tst{

class runners_pool{
	std::vector<std::unique_ptr<runner>> runners;
	std::vector<runner*> free_runners;
public:
	void stop_all_runners(){
		for(auto& r : this->runners){
			r->stop();
		}
	}

	~runners_pool(){
		for(auto& r : this->runners){
			r->join();
		}
	}

	void free_runner(runner* r){
		ASSERT(std::find(this->free_runners.begin(), this->free_runners.end(), r) == this->free_runners.end(), [](auto&o){o << "runner is already freed";})
		this->free_runners.push_back(r);
	}

	runner* occupy_runner();

	bool no_active_runners()const noexcept{
		return this->runners.size() == this->free_runners.size();
	}
};

}

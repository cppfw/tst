#ifndef TST_NO_PAR

#include "runners_pool.hxx"

#include "settings.hxx"

using namespace tst;

runner* runners_pool::occupy_runner(){
	if(!this->free_runners.empty()){
		auto r = this->free_runners.back();
		ASSERT(r)
		this->free_runners.pop_back();
		return r;
	}else if(this->runners.size() != settings::inst().num_threads){
		ASSERT(this->runners.size() < settings::inst().num_threads)
		this->runners.push_back(std::make_unique<runner>());
		auto r = this->runners.back().get();
		r->start();
		return r;
	}
	return nullptr;
}

#endif // ~TST_NO_PAR

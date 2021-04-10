#include "runner.hxx"

using namespace tst;

void runner::stop(){
	this->queue.push_back([this](){
		this->quit = true;
	});
}

void runner::run(){
	opros::wait_set wait_set(1);

	wait_set.add(this->queue, {opros::ready::read});
	utki::scope_exit queue_scope_exit([&](){
		wait_set.remove(this->queue);
	});

	while(!this->quit){
		wait_set.wait();

		ASSERT(this->queue.flags().get(opros::ready::read))
		auto f = this->queue.pop_front();
		ASSERT(f)

		f();
	}
}

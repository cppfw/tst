#pragma once

#include <nitki/thread.hpp>
#include <nitki/queue.hpp>

namespace tst{

class runner : public nitki::thread{
	bool quit = false;
public:
	nitki::queue queue;

	void stop();

	void run()override;
};

}

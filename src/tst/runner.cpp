/*
MIT License

Copyright (c) 2021 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#ifndef TST_NO_PAR

#	include "runner.hxx"

using namespace tst;

void runner::stop()
{
	this->queue.push_back([this]() {
		this->quit = true;
	});
}

void runner::run()
{
	opros::wait_set wait_set(1);

	wait_set.add(this->queue, {opros::ready::read});
	utki::scope_exit queue_scope_exit([&]() {
		wait_set.remove(this->queue);
	});

	while (!this->quit) {
#	ifdef DEBUG
		auto num_triggered =
#	endif
			wait_set.wait(nullptr);

		ASSERT(num_triggered == 1)
		auto f = this->queue.pop_front();
		ASSERT(f)

		f();
	}
}

#endif // ~TST_NO_PAR

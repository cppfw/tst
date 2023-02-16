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

#	include "runners_pool.hxx"

#	include "settings.hxx"

using namespace tst;

runner* runners_pool::occupy_runner()
{
	if (!this->free_runners.empty()) {
		auto r = this->free_runners.back();
		ASSERT(r)
		this->free_runners.pop_back();
		return r;
	} else if (this->runners.size() != settings::inst().num_threads) {
		ASSERT(this->runners.size() < settings::inst().num_threads)
		this->runners.push_back(std::make_unique<runner>());
		auto r = this->runners.back().get();
		r->start();
		return r;
	}
	return nullptr;
}

#endif // ~TST_NO_PAR

/*
MIT License

Copyright (c) 2021-2025 Ivan Gagis

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

#pragma once

#include <vector>

#include "runner.hxx"

namespace tst {

class runners_pool
{
	std::vector<std::unique_ptr<runner>> runners;
	std::vector<runner*> free_runners;

public:
	void stop_all_runners()
	{
		for (auto& r : this->runners) {
			r->quit();
		}
	}

	runners_pool() = default;

	runners_pool(const runners_pool&) = delete;
	runners_pool& operator=(const runners_pool&) = delete;

	runners_pool(runners_pool&&) = delete;
	runners_pool& operator=(runners_pool&&) = delete;

	~runners_pool()
	{
		for (auto& r : this->runners) {
			r->join();
		}
	}

	void free_runner(runner* r)
	{
		ASSERT(
			std::find(this->free_runners.begin(), this->free_runners.end(), r) == this->free_runners.end(),
			[](auto& o) {
				o << "runner is already freed";
			}
		)
		this->free_runners.push_back(r);
	}

	runner* occupy_runner();

	bool no_active_runners() const noexcept
	{
		return this->runners.size() == this->free_runners.size();
	}
};

} // namespace tst

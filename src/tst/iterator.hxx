/*
MIT License

Copyright (c) 2021-2023 Ivan Gagis

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

#include "application.hpp"
#include "util.hxx"

namespace tst {

class iterator
{
	const decltype(application::suites) & suites;

	decltype(application::suites)::const_iterator si;
	decltype(suite::tests)::const_iterator pi;

	void init_pi()
	{
		for (; this->si != this->suites.end(); ++this->si) {
			if (!this->si->second.tests.empty()) {
				this->pi = this->si->second.tests.begin();
				return;
			}
		}
	}

public:
	iterator(decltype(suites) & suites) :
		suites(suites),
		si(suites.begin())
	{
		this->init_pi();
	}

	bool is_valid() const
	{
		return this->si != this->suites.end();
	}

	void next()
	{
		ASSERT(this->is_valid())
		++this->pi;
		if (this->pi == this->si->second.tests.end()) {
			++this->si;
			this->init_pi();
		}
	}

	const suite::test_info& info() const
	{
		ASSERT(this->is_valid())
		return this->pi->second;
	}

	full_id id() const
	{
		ASSERT(this->is_valid())
		// NOLINTNEXTLINE(modernize-use-designated-initializers)
		return {this->si->first, this->pi->first};
	}
};

} // namespace tst

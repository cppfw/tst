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

#include "suite.hpp"

#include "settings.hxx"
#include "util.hxx"

using namespace tst;

void suite::add(const std::string& id, utki::flags<flag> flags, std::function<void()>&& proc)
{
	validate_id(id);

	if (!proc) {
		throw std::invalid_argument("test procedure is nullptr");
	}

	if (settings::inst().run_disabled) {
		flags.clear(flag::disabled);
	}

	auto r = this->tests.insert(std::make_pair(id, test_info{std::move(proc), flags}));
	if (!r.second) {
		std::stringstream ss;
		ss << "test with id = '" << id << "' already exists in the test suite";
		throw std::invalid_argument(ss.str());
	}
}

void suite::add_disabled(const std::string& id, utki::flags<flag> flags, std::function<void()>&& proc)
{
	flags.set(flag::disabled);
	this->add(id, flags, std::move(proc));
}

const char* suite::status_to_string(status s)
{
	switch (s) {
		case status::passed:
			return "passed";
		case status::disabled:
			return "disabled";
		case status::errored:
			return "errored";
		case status::failed:
			return "failed";
		case status::not_run:
			return "not run";
	}

	ASSERT(false)
	return nullptr;
}

std::string suite::make_indexed_id(const std::string& id, size_t index)
{
	std::stringstream ss;
	ss << id << "[" << index << "]";
	return ss.str();
}

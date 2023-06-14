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

#include "check.hpp"

#include "util.hxx"

using namespace tst;

namespace {
const char* default_fail_message = "check(false)";
} // namespace

void tst::check(bool c, const std::function<void(std::ostream&)>& print, utki::source_location&& source_location)
{
	if (c) {
		return;
	}

	std::stringstream ss;

	if (print) {
		print(ss);
	} else {
		ss << default_fail_message;
	}

	throw check_failed(ss.str(), std::move(source_location));
}

// TODO: why lint complains about it on macos?
// "error: an exception may be thrown"
// NOLINTNEXTLINE(bugprone-exception-escape)
check_result::check_result(check_result&& cr) noexcept :
	failed(cr.failed),
	source_location(std::move(cr.source_location)),
	ss(std::move(cr.ss))
{
	cr.failed = false;
}

// TODO: remove lint suppression when
// https://github.com/llvm/llvm-project/issues/55143 is resolved
// NOLINTNEXTLINE(bugprone-exception-escape)
check_result::~check_result() noexcept(false)
{
	if (!this->failed) {
		return;
	}

	std::string message;
	try {
		message = this->ss.str();
		if (message.empty()) {
			message = default_fail_message;
		}
	} catch (...) {
	}
	throw check_failed(std::move(message), std::move(this->source_location));
}

check_result tst::check(bool c, utki::source_location&& source_location)
{
#ifdef DEBUG
	// This piece of code is just to test move constructor of check_result,
	// we do it here instead of writing normal tst tests because check_result
	// has private constructors in order to prevent misuse from user side,
	// because check_result throws from destructor and should not be normally
	// constructed by users explicitly to prevent unexpected behavior.
	{
		check_result cr(std::move(source_location));
		ASSERT(cr.failed)

		check_result mcr{std::move(cr)};
		ASSERT(mcr.failed)
		ASSERT(!cr.failed)

		mcr.failed = false;
	}
#endif

	if (c) {
		return {};
	}

	return std::move(source_location);
}

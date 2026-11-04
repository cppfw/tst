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

#include <cstddef>
#include <functional>
#include <sstream>
#include <utility>

#include <utki/debug.hpp>
#include <utki/unicode.hpp>

namespace tst {

/**
 * @brief Check for condition with additional failure information.
 * The function checks for the condition to be true.
 * In case the condition is true, the function does nothing.
 * In case the condition is false, the function prepares a failure message by
 * calling the provided function and throws an exception containing the failure
 * information.
 * @param c - condition to check.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
void check(
	bool c, //
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
);

/**
 * @brief Template check() function for any type convertible to bool.
 * This template converts the given value to boolean and then passes it to
 * check(bool, print, source_location) overload.
 * @param p - value to convert to boolean and check for true-value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class check_type>
void check(
	const check_type& p, //
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		static_cast<bool>(p), //
		print,
		std::move(source_location)
	);
}

/**
 * @brief Check for condition.
 * The function checks for the condition to be true. In case the condition is
 * false, the function throws a check failure exception.
 * @param c - condition to check.
 * @param source_location - object with source file:line information.
 */
void check(
	bool c,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
);

/**
 * @brief Template check() function for any type convertible to bool.
 * This template converts the given value to boolean and then passes it to
 * check(bool, source_location) overload.
 * @param p - value to convert to boolean and check for true-value.
 * @param source_location - object with source file:line information.
 */
template <class check_type>
void check(
	const check_type& p,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		static_cast<bool>(p), //
		std::move(source_location)
	);
}

namespace internal {
/**
 * @brief Stream a value into an output stream.
 * This helper provides special handling for UTF-32 strings, which cannot be
 * streamed directly into a std::ostream. They are converted to UTF-8 first.
 * @param o - output stream.
 * @param v - value to stream.
 */
inline void stream_value(std::ostream& o, const std::u32string& v)
{
	o << utki::to_utf8(v);
}

inline void stream_value(std::ostream& o, const std::u32string_view& v)
{
	o << utki::to_utf8(utki::make_span(v.data(), v.size()));
}

template <class value_type>
inline void stream_value(std::ostream& o, const value_type& v)
{
	o << v;
}
} // namespace internal

/**
 * @brief Check for equality.
 * This is a convenience function which checks for equality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints
 * the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_eq(
	const parameter_type& a,
	const parameter_type& b,
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		a == b,
		[&](auto& o) {
			o << "check_eq(";
			internal::stream_value(o, a);
			o << ", ";
			internal::stream_value(o, b);
			o << ")";
			if (print) {
				print(o);
			}
		},
		std::move(source_location)
	);
}

/**
 * @brief Check for equality.
 * This is a convenience function which checks for equality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints
 * the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_eq(
	const parameter_type& a, //
	const parameter_type& b,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check_eq(a, b, nullptr, std::move(source_location));
}

/**
 * @brief Check for inequality.
 * This is a convenience function which checks for inequality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints
 * the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_ne(
	const parameter_type& a, //
	const parameter_type& b,
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		a != b,
		[&](auto& o) {
			o << "check_ne(";
			internal::stream_value(o, a);
			o << ", ";
			internal::stream_value(o, b);
			o << ")";
			if (print) {
				print(o);
			}
		},
		std::move(source_location)
	);
}

/**
 * @brief Check for inequality.
 * This is a convenience function which checks for inequality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints
 * the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_ne(
	const parameter_type& a, //
	const parameter_type& b,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check_ne(a, b, nullptr, std::move(source_location));
}

/**
 * @brief Check for less than.
 * This is a convenience function which checks for one value being less than
 * another value. Under the hood it calls to tst::check(), but also, it
 * automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_lt(
	const parameter_type& a, //
	const parameter_type& b,
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		a < b,
		[&](auto& o) {
			o << "check_lt(";
			internal::stream_value(o, a);
			o << ", ";
			internal::stream_value(o, b);
			o << ")";
			if (print) {
				print(o);
			}
		},
		std::move(source_location)
	);
}

/**
 * @brief Check for less than.
 * This is a convenience function which checks for one value being less than
 * another value. Under the hood it calls to tst::check(), but also, it
 * automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_lt(
	const parameter_type& a, //
	const parameter_type& b,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check_lt(a, b, nullptr, std::move(source_location));
}

/**
 * @brief Check for greater than.
 * This is a convenience function which checks for one value being greater than
 * another value. Under the hood it calls to tst::check(), but also, it
 * automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_gt(
	const parameter_type& a, //
	const parameter_type& b,
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		a > b,
		[&](auto& o) {
			o << "check_gt(";
			internal::stream_value(o, a);
			o << ", ";
			internal::stream_value(o, b);
			o << ")";
			if (print) {
				print(o);
			}
		},
		std::move(source_location)
	);
}

/**
 * @brief Check for greater than.
 * This is a convenience function which checks for one value being greater than
 * another value. Under the hood it calls to tst::check(), but also, it
 * automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_gt(
	const parameter_type& a, //
	const parameter_type& b,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check_gt(a, b, nullptr, std::move(source_location));
}

/**
 * @brief Check for less than or equal.
 * This is a convenience function which checks for one value being less than or
 * equal to another value. Under the hood it calls to tst::check(), but also, it
 * automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_le(
	const parameter_type& a, //
	const parameter_type& b,
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		a <= b,
		[&](auto& o) {
			o << "check_le(";
			internal::stream_value(o, a);
			o << ", ";
			internal::stream_value(o, b);
			o << ")";
			if (print) {
				print(o);
			}
		},
		std::move(source_location)
	);
}

/**
 * @brief Check for less than or equal.
 * This is a convenience function which checks for one value being less than or
 * equal to another value. Under the hood it calls to tst::check(), but also, it
 * automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_le(
	const parameter_type& a, //
	const parameter_type& b,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check_le(a, b, nullptr, std::move(source_location));
}

/**
 * @brief Check for greater than or equal.
 * This is a convenience function which checks for one value being greater than
 * or equal to another value. Under the hood it calls to tst::check(), but also,
 * it automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message
 * information.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_ge(
	const parameter_type& a, //
	const parameter_type& b,
	const std::function<void(std::ostream&)>& print,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check(
		a >= b, //
		[&](auto& o) {
			o << "check_ge(";
			internal::stream_value(o, a);
			o << ", ";
			internal::stream_value(o, b);
			o << ")";
			if (print) {
				print(o);
			}
		},
		std::move(source_location)
	);
}

/**
 * @brief Check for greater than or equal.
 * This is a convenience function which checks for one value being greater than
 * or equal to another value. Under the hood it calls to tst::check(), but also,
 * it automatically prints the input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter_type>
void check_ge(
	const parameter_type& a, //
	const parameter_type& b,
	utki::source_location source_location
#if CFG_CPP >= 20
	= utki::std_source_location::current()
#endif
)
{
	check_ge(a, b, nullptr, std::move(source_location));
}

} // namespace tst

#pragma once

#include <cstddef>
#include <utility>
#include <sstream>
#include <functional>

#include <utki/debug.hpp>

namespace tst{

/**
 * @brief Check for condition with additional failure information.
 * The function checks for the condition to be true.
 * In case the condition is true, the function does nothing.
 * In case the condition is false, the function prepares a failure message by
 * calling the provided function and throws an exception containing the failure information.
 * @param c - condition to check.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
void check(
		bool c,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	);

// smart pointers have explicit 'operator bool()', so we need to add oveloads for those

template <class type>
void check(
		const std::shared_ptr<type>& p,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	)
{
	check(p != nullptr, print, source_location);
}

template <class type>
void check(
		const std::unique_ptr<type>& p,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	)
{
	check(p != nullptr, print, source_location);
}

template <class type>
void check(
		const std::function<type>& p,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	)
{
	check(p != nullptr, print, source_location);
}

#define CHECK_INTERNAL2(condition, print) tst::check((condition), (print), SL)

/**
 * @brief Check for condition.
 * The function checks for the condition to be true.
 * In case the condition is true, the function does nothing.
 * In case the condition is false, the function throws an exception.
 * @param c - condition to check.
 * @param source_location - object with source file:line information.
 */
template <class type>
void check(
		const type& c,
		const utki::source_location& source_location
	)
{
	check(c, nullptr, source_location);
}

#define CHECK_INTERNAL1(condition) tst::check((condition), SL)

/**
 * @brief Check for condition macro.
 * This is a convenience macro which wraps a call to tst::check() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, _3, CHECK_INTERNAL2, CHECK_INTERNAL1)(__VA_ARGS__)

/**
 * @brief Check for equality.
 * This is a convenience function which checks for equality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically print the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	)
{
	check(
			a == b,
			[&](auto& o){
				o << "check_eq(" << a << ", " << b << ")";
				if(print){
					o << ": ";
					print(o);
				}
			},
			source_location
		);
}

#define CHECK_EQ_INTERNAL3(a, b, print) tst::check_eq((a), (b), (print), SL)

/**
 * @brief Check for equality.
 * This is a convenience function which checks for equality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically print the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const utki::source_location& source_location
	)
{
	check_eq(a, b, nullptr, source_location);
}

#define CHECK_EQ_INTERNAL2(a, b) tst::check_eq((a), (b), SL)

/**
 * @brief Check for equality macro.
 * This is a convenience macro which wraps a call to tst::check_eq() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_EQ(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_EQ_INTERNAL3, CHECK_EQ_INTERNAL2)(__VA_ARGS__)

/**
 * @brief Check for inequality.
 * This is a convenience function which checks for inequality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically print the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_ne(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		const utki::source_location& source_location
	)
{
	check(
			a != b,
			[&](auto& o){
				o << "check_ne(" << a << ", " << b << ")";
				if(print){
					o << ": ";
					print(o);
				}
			},
			source_location
		);
}

#define CHECK_NE_INTERNAL3(a, b, print) tst::check_ne((a), (b), (print), SL)

/**
 * @brief Check for inequality.
 * This is a convenience function which checks for inequality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically print the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_ne(
		const parameter& a,
		const parameter& b,
		const utki::source_location& source_location
	)
{
	check_ne(a, b, nullptr, source_location);
}

#define CHECK_NE_INTERNAL2(a, b) tst::check_ne((a), (b), SL)

/**
 * @brief Check for inequality macro.
 * This is a convenience macro which wraps a call to tst::check_ne() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_NE(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_NE_INTERNAL3, CHECK_NE_INTERNAL2)(__VA_ARGS__)


}
